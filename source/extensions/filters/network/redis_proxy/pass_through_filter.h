#pragma once

#include <cstdint>
#include <list>
#include <memory>
#include <string>

#include "envoy/extensions/filters/network/redis_proxy/v3/redis_proxy.pb.h"
#include "envoy/network/drain_decision.h"
#include "envoy/network/filter.h"
#include "envoy/stats/scope.h"
#include "envoy/upstream/cluster_manager.h"

#include "source/common/buffer/buffer_impl.h"
#include "source/extensions/filters/network/common/redis/codec.h"
#include "source/extensions/filters/network/redis_proxy/command_splitter.h"

namespace Envoy {
namespace Extensions {
namespace NetworkFilters {
namespace RedisProxy {

/**
 * Configuration for the redis proxy filter.
 */
class PassThroughConfig {
public:
  PassThroughConfig(const envoy::extensions::filters::network::redis_proxy::v3::RedisProxy_AdaptiveConcurrency& config,
                    const Network::DrainDecision& drain_decision,
                    Runtime::Loader& runtime, std::string stats_prefix, Stats::Scope& scope,  TimeSource& time_source, 
                    Api::Api& api);

  bool filterEnabled() const { return adaptive_concurrency_feature_.enabled(); }
  TimeSource& timeSource() const { return time_source_; }

  const Network::DrainDecision& drain_decision_;
  Runtime::Loader& runtime_;
  const std::string redis_drain_close_runtime_key_{"redis.drain_close_enabled"};

private:
  const std::string stats_prefix_;
  TimeSource& time_source_;
  Runtime::FeatureFlag adaptive_concurrency_feature_;
};

using PassThroughConfigSharedPtr = std::shared_ptr<PassThroughConfig>;

/**
 * A redis multiplexing proxy filter. This filter will take incoming redis pipelined commands, and
 * multiplex them onto a consistently hashed connection pool of backend servers.
 */
class PassThroughFilter : public Network::ReadFilter,
                          public Common::Redis::DecoderCallbacks,
                          public Network::ConnectionCallbacks,
                          Logger::Loggable<Logger::Id::filter> {
public:
  PassThroughFilter(Common::Redis::DecoderFactory& factory, Common::Redis::EncoderPtr&& encoder,
              CommandSplitter::Instance& splitter, PassThroughConfigSharedPtr config);
  ~PassThroughFilter() override;

  // Network::ReadFilter
  void initializeReadFilterCallbacks(Network::ReadFilterCallbacks& callbacks) override;
  Network::FilterStatus onData(Buffer::Instance& data, bool end_stream) override;
  Network::FilterStatus onNewConnection() override { return Network::FilterStatus::Continue; }

  // Network::ConnectionCallbacks
  void onEvent(Network::ConnectionEvent event) override;
  void onAboveWriteBufferHighWatermark() override {}
  void onBelowWriteBufferLowWatermark() override {}

  // Common::Redis::DecoderCallbacks
  void onRespValue(Common::Redis::RespValuePtr&& value) override;

  bool connectionAllowed() { return connection_allowed_; }

private:
  friend class PassThroughFilterTest;

  struct PendingRequest : public CommandSplitter::SplitCallbacks {
    PendingRequest(ProxyFilter& parent);
    ~PendingRequest() override;

    // RedisProxy::CommandSplitter::SplitCallbacks
    bool connectionAllowed() override { return parent_.connectionAllowed(); }
    void onAuth(const std::string& password) override { parent_.onAuth(*this, password); }
    void onAuth(const std::string& username, const std::string& password) override {
      parent_.onAuth(*this, username, password);
    }
    void onResponse(Common::Redis::RespValuePtr&& value) override {
      parent_.onResponse(*this, std::move(value));
    }

    PassThroughFilter& parent_;
    Common::Redis::RespValuePtr pending_response_;
    CommandSplitter::SplitRequestPtr request_handle_;
  };

  void onAuth(PendingRequest& request, const std::string& password);
  void onAuth(PendingRequest& request, const std::string& username, const std::string& password);
  void onResponse(PendingRequest& request, Common::Redis::RespValuePtr&& value);

  Common::Redis::DecoderPtr decoder_;
  Common::Redis::EncoderPtr encoder_;
  CommandSplitter::Instance& splitter_;
  PassThroughConfigSharedPtr config_;
  Buffer::OwnedImpl encoder_buffer_;
  Network::ReadFilterCallbacks* callbacks_{};
  std::list<PendingRequest> pending_requests_;
  bool connection_allowed_;
};

} // namespace RedisProxy
} // namespace NetworkFilters
} // namespace Extensions
} // namespace Envoy
