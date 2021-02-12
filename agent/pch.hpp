/**
 * @file pch.hpp
 * @brief Pre-Compiled Header
 *
 */

#pragma once

/* StdLib Stuff */
#include <set>
#include <regex>
#include <mutex>
#include <atomic>
#include <string>
#include <thread>
#include <chrono>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <filesystem>
#include <functional>
#include <condition_variable>

/* Application Config */
#include "config.h"

/* Horus Logger */
#include "core/logger.h"
#include "debug/assert.h"
#include "debug/instrumentor.h"
