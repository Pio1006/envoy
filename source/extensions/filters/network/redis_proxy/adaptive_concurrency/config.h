#pragma once

#include "envoy/extensions/filters/network/redis_proxy/v3/adaptive_concurrency.pb.h"
#include "envoy/extensions/filters/network/redis_proxy/v3/adaptive_concurrency.pb.validate.h"

// #include "extensions/filters/http/common/factory_base.h"
//#include "extensions/filters/http/well_known_names.h"
#include "extensions/filters/network/common/factory_base.h"
#include "extensions/filters/network/well_known_names.h"

namespace Envoy {
namespace Extensions {
namespace NetworkFilters {
namespace RedisProxy {
namespace AdaptiveConcurrency {

/**
 * Config registration for the adaptive concurrency limit filter. @see NamedHttpFilterConfigFactory.
 */
class AdaptiveConcurrencyFilterFactory
    : public Common::FactoryBase<
          envoy::extensions::filters::network::redis_proxy::v3::AdaptiveConcurrency> {
public:
  AdaptiveConcurrencyFilterFactory() : FactoryBase(NetworkFilterNames::get().AdaptiveConcurrency) {}

  Network::FilterFactoryCb createFilterFactoryFromProtoTyped(
      const envoy::extensions::filters::network::redis_proxy::v3::AdaptiveConcurrency&
          proto_config,
      const std::string& stats_prefix, Server::Configuration::FactoryContext& context) override;
};

} // namespace AdaptiveConcurrency
} // namespace RedisProxy
} // namespace NetworkFilters
} // namespace Extensions
} // namespace Envoy

