// Copyright(c) 2015-present, Gabi Melman & spdlog contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "spdlog/sinks/dist_sink.h"
#include <spdlog/details/null_mutex.h>
#include <spdlog/details/log_msg.h>

#include <mutex>
#include <string>
#include <chrono>

// Duplicate message removal sink.
// Skip the message if previous one is a duplicate based on a distance function
// and less than "max_skip_duration" have passed
//
// Example:
//
//     #include <spdlog/sinks/dup_dist_filter_sink.h>
//
//     int main() {
//         auto dup_filter = std::make_shared<dup_dist_filter_sink_st>(std::chrono::seconds(5), 10);
//         dup_filter->add_sink(std::make_shared<stdout_color_sink_mt>());
//         spdlog::logger l("logger", dup_filter);
//         l.info("Hello");
//         l.info("Hello");
//         l.info("Hello");
//         l.info("Different Hello");
//     }
//
// Will produce:
//       [2019-06-25 17:50:56.511] [logger] [info] Hello 1
//       [2019-06-25 17:50:56.512] [logger] [info] Skipped 3 duplicate messages..
//       [2019-06-25 17:50:56.512] [logger] [info] Different Hello

namespace spdlog::sinks {


template<typename Mutex>
class dup_dist_filter_sink : public dist_sink<Mutex>
{
public:
    template<class Rep, class Period>
    explicit dup_dist_filter_sink(std::chrono::duration<Rep, Period> max_skip_duration, unsigned distance = 10)
        : max_skip_duration_{max_skip_duration}, distance_{distance}
    {}

protected:
    std::chrono::microseconds max_skip_duration_;
    unsigned distance_;
    log_clock::time_point last_msg_time_;
    std::string last_msg_payload_;
	level::level_enum last_msg_level_;
	std::string last_msg_logger_name_;
	size_t skip_counter_ = 0;


    void sink_it_(const details::log_msg &msg) override
    {
        bool filtered = filter_(msg);
        if (!filtered)
        {
            skip_counter_ += 1;
            return;
        }

        // log the "skipped.." message
        if (skip_counter_ > 0)
        {
            memory_buf_t buf;
            fmt::format_to(buf, "+--> skipped {} messages similar to '{}'", skip_counter_, last_msg_payload_);
            details::log_msg skipped_msg{last_msg_logger_name_, last_msg_level_, string_view_t{buf.data(), buf.size()}};
            dist_sink<Mutex>::sink_it_(skipped_msg);
        }

        // log current message
        dist_sink<Mutex>::sink_it_(msg);
        last_msg_time_ = msg.time;
        last_msg_payload_.assign(msg.payload.data(), msg.payload.data() + msg.payload.size());
        last_msg_level_ = msg.level;
		last_msg_logger_name_.assign(msg.logger_name.data(), msg.logger_name.data() + msg.logger_name.size());
		skip_counter_ = 0;
    }

    // return whether the log msg should be displayed (true) or skipped (false)
    bool filter_(const details::log_msg &msg)
    {
        auto filter_duration = msg.time - last_msg_time_;

        auto s1 = msg.payload;
        auto s2 = last_msg_payload_;

		// Distance function ========================
		// From https://en.wikibooks.org/wiki/Algorithm_Implementation/Strings/Levenshtein_distance#C++
		// https://en.wikibooks.org/wiki/Wikibooks:Copyrights
		const std::size_t len1 = s1.size(), len2 = s2.size();
		std::vector<std::vector<unsigned int>> d(len1 + 1, std::vector<unsigned int>(len2 + 1));

		d[0][0] = 0;
		for(unsigned int i = 1; i <= len1; ++i) d[i][0] = i;
		for(unsigned int i = 1; i <= len2; ++i) d[0][i] = i;

		for(unsigned int i = 1; i <= len1; ++i)
			for(unsigned int j = 1; j <= len2; ++j)
				// note that std::min({arg1, arg2, arg3}) works only in C++11,
				// for C++98 use std::min(std::min(arg1, arg2), arg3)
				d[i][j] = (std::min)({ d[i - 1][j] + 1, d[i][j - 1] + 1, d[i - 1][j - 1] + (s1[i - 1] == s2[j - 1] ? 0 : 1) });
		auto distance = d[len1][len2];
		// ===========================================
		//std::cout << "Distance " << distance << std::endl;
		return (filter_duration > max_skip_duration_) || (distance > distance_);
    }
};

using dup_dist_filter_sink_mt = dup_dist_filter_sink<std::mutex>;
using dup_dist_filter_sink_st = dup_dist_filter_sink<details::null_mutex>;

} // namespace spdlog::sinks

