cmake_minimum_required(VERSION 3.8.2)

include($ENV{NCS_BASE}/nrf/cmake/boilerplate.cmake NO_POLICY_SCOPE)
include($ENV{ZEPHYR_BASE}/cmake/app/boilerplate.cmake NO_POLICY_SCOPE)
project(udp_con_test)

target_sources(app PRIVATE src/contest.c)
