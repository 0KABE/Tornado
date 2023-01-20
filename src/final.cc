//
// Created by Chen, WenTao on 2023/1/12.
//

#include "final.h"

Tornado::Final::Final(std::function<void()> action) : action_(std::move(action)) {}

Tornado::Final::~Final() { action_(); }
