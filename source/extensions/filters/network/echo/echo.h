#pragma once

#include "envoy/network/filter.h"

#include "source/common/common/logger.h"

namespace Envoy {
namespace Extensions {
namespace NetworkFilters {
namespace Echo {

/**
 * Implementation of a basic echo filter.
 */
class EchoFilter : public Network::Filter, Logger::Loggable<Logger::Id::filter> {
public:
  // Network::ReadFilter
  Network::FilterStatus onData(Buffer::Instance& data, bool) override;
  Network::FilterStatus onNewConnection() override { return Network::FilterStatus::Continue; }

  Network::FilterStatus onWrite(Buffer::Instance& data, bool) override;
  void initializeReadFilterCallbacks(Network::ReadFilterCallbacks& readCallbacks) override {
    read_callbacks_ = &readCallbacks;
  }
  void initializeWriteFilterCallbacks(Network::WriteFilterCallbacks& writeCallbacks) override {
    write_callbacks_ = &writeCallbacks;
  }

private:
  Network::ReadFilterCallbacks* read_callbacks_{};
  Network::WriteFilterCallbacks* write_callbacks_{};
};

} // namespace Echo
} // namespace NetworkFilters
} // namespace Extensions
} // namespace Envoy
