// Copyright 2018 The Beam Team
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "bbs/bbs_client.h"
#include "utility/logger.h"
#include "utility/helpers.h"
#include "utility/asynccontext.h"

using namespace beam;
using namespace beam::bbs;

void test1() {
    try {
        auto reactor = io::Reactor::create();
        io::Reactor::Scope s{ *reactor };
        std::unique_ptr<Client> client(Client::create());
        client->connect(io::Address::localhost().port(2020), [](io::ErrorCode status){});
    } catch (...) {
#ifndef WIN32
        kill(0, SIGTERM);
#endif // !WIN32
        
    }
}

int main(int argc, char* argv[]) {
    int logLevel = LOG_LEVEL_DEBUG;
#if LOG_VERBOSE_ENABLED
    logLevel = LOG_LEVEL_VERBOSE;
#endif
    auto logger = Logger::create(logLevel, logLevel, LOG_LEVEL_INFO, "bbs_");
    logger->set_header_formatter(
        [](char* buf, size_t maxSize, const char* timestampFormatted, const LogMessageHeader& header) -> size_t {
            if (header.line)
                return snprintf(buf, maxSize, "%c %s (%s) ", loglevel_tag(header.level), timestampFormatted, header.func);
            return snprintf(buf, maxSize, "%c %s ", loglevel_tag(header.level), timestampFormatted);
        }
    );
    logger->set_time_format("%T", true);

/*
    if (argc < 4) {
        LOG_ERROR() << "Usage: ./bbs PORT SPECTRE DEPTH [CONNECT_TO]";
        return 1;
    }

    bbs::Config config;
    config.serverAddress.port(atoi(argv[1]));
    config.spectre = strtol(argv[2], 0, 16);
    config.historyDepth = atoi(argv[3]);

    std::vector<io::Address> connectTo;

    if (argc >= 5) {
        io::Address addr;
        if (!strchr(argv[4], ':') || !addr.resolve(argv[4]) || addr.ip() == 0) {
            LOG_ERROR() << "Invalid peer address " << argv[4];
            return 1;
        }
        connectTo.push_back(addr);
    }
*/

    try {
        AsyncContext ctx;
        ctx.run_async(test1);
        wait_for_termination(0);
        ctx.stop();
        ctx.wait();
        return 0;
    } catch (const std::exception& e) {
        LOG_ERROR() << "Exception: " << e.what();
    } catch (...) {
        LOG_ERROR() << "Non-std exception";
    }

    return 127;
}
