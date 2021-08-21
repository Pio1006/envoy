#include "source/extensions/filters/network/redis_proxy/pass_through_filter.h"

#include <cstdint>
#include <string>

#include "envoy/extensions/filters/network/redis_proxy/v3/redis_proxy.pb.h"
#include "envoy/stats/scope.h"

#include "source/common/common/assert.h"
#include "source/common/common/fmt.h"
#include "source/common/config/datasource.h"
#include "source/common/config/utility.h"

namespace Envoy {
namespace Extensions {
namespace NetworkFilters {
namespace RedisProxy {

PassThroughConfig::PassThroughConfig(
    const envoy::extensions::filters::network::redis_proxy::v3::RedisProxy_AdaptiveConcurrency& config,
    const Network::DrainDecision& drain_decision, 
    Runtime::Loader& runtime,
    std::string stats_prefix,
    Stats::Scope& scope,
    TimeSource& time_source,
    Api::Api& api)
    : stats_prefix_(std::move(stats_prefix)), time_source_(time_source),
      adaptive_concurrency_feature_(proto_config.enabled(), runtime), 
      drain_decision_(drain_decision), runtime_(runtime) {}


PassThroughFilter::PassThroughFilter(
  Common::Redis::DecoderFactory& factory,
  Common::Redis::EncoderPtr&& encoder, CommandSplitter::Instance& splitter,
  PassThroughConfigSharedPtr config)
    : decoder_(factory.create(*this)), encoder_(std::move(encoder)), splitter_(splitter),
      config_(std::move(config)) {}

PassThroughFilter::~PassThroughFilter() {
  ASSERT(pending_requests_.empty());
}

void PassThroughFilter::initializeReadFilterCallbacks(Network::ReadFilterCallbacks& callbacks) {
  callbacks_ = &callbacks;
  callbacks_->connection().addConnectionCallbacks(*this);
}

void PassThroughFilter::onRespValue(Common::Redis::RespValuePtr&& value) {
  ENVOY_LOG(debug"in PassThroughFilter::onRespValue");
}


void ProxyFilter::onResponse(PendingRequest& request, Common::Redis::RespValuePtr&& value) {
  ENVOY_LOG(debug"in ProxyFilter::onResponse");
}

Network::FilterStatus ProxyFilter::onData(Buffer::Instance& data, bool) {
  try {
    decoder_->decode(data);
    return Network::FilterStatus::Continue;
  } catch (Common::Redis::ProtocolError&) {
    config_->stats_.downstream_cx_protocol_error_.inc();
    Common::Redis::RespValue error;
    error.type(Common::Redis::RespType::Error);
    error.asString() = "downstream protocol error";
    encoder_->encode(error, encoder_buffer_);
    callbacks_->connection().write(encoder_buffer_, false);
    callbacks_->connection().close(Network::ConnectionCloseType::NoFlush);
    return Network::FilterStatus::StopIteration;
  }
}
} // namespace RedisProxy
} // namespace NetworkFilters
} // namespace Extensions
} // namespace Envoy
