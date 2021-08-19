#pragma once

// #include "envoy/extensions/filters/network/adaptive_concurrency/v3/adaptive_concurrency.pb.h"
// #include "envoy/extensions/filters/network/adaptive_concurrency/v3/adaptive_concurrency.pb.validate.h"
#include "envoy/extensions/filters/network/redis_proxy/v3/redis_proxy.pb.h"
#include "envoy/extensions/filters/network/redis_proxy/v3/redis_proxy.pb.validate.h"


// #include "extensions/filters/http/common/factory_base.h"
// #include "extensions/filters/http/well_known_names.h"
#include "extensions/filters/network/common/factory_base.h"
#include "extensions/filters/network/well_known_names.h"

namespace Envoy {
namespace Extensions {
namespace NetworkFilters {
namespace Common {
namespace Redis {
namespace AdaptiveConcurrency {

/**
 * Config registration for the adaptive concurrency limit filter. @see NamedHttpFilterConfigFactory.
 */
class AdaptiveConcurrencyFilterFactory
    : public Common::FactoryBase<
          envoy::extensions::filters::network::redis_proxy::v3::RedisProxy_AdaptiveConcurrency> {
public:
  AdaptiveConcurrencyFilterFactory() : FactoryBase(NetworkFilterNames::get().AdaptiveConcurrency) {}

  Network::FilterFactoryCb createFilterFactoryFromProtoTyped(
      const envoy::extensions::filters::network::redis_proxy::v3::RedisProxy_AdaptiveConcurrency&
          proto_config,
      const std::string& stats_prefix, Server::Configuration::FactoryContext& context) override;
};

} // namespace AdaptiveConcurrency
} // namespace Redis
} // namespace Common
} // namespace NetworkFilters
} // namespace Extensions
} // namespace Envoy
