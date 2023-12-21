/*
 * Copyright 2022 Barabas Raffai
 *
 * This file is part of ShrapnelDSP.
 *
 * ShrapnelDSP is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * ShrapnelDSP is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * ShrapnelDSP. If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include "api.h"
#include <cstdint>
#include <etl/array.h>
#include <etl/string.h>
#include <etl/string_stream.h>
#include <variant>

#include "presets.h"
#include "presets.pb.h"

namespace shrapnel::presets {

struct ParametersData
{
    float amp_gain;
    float amp_channel;
    float bass;
    float middle;
    float treble;
    float contour;
    float volume;
    float noise_gate_threshold;
    float noise_gate_hysteresis;
    float noise_gate_attack;
    float noise_gate_hold;
    float noise_gate_release;
    float noise_gate_bypass;
    float chorus_rate;
    float chorus_depth;
    float chorus_mix;
    float chorus_bypass;
    float wah_position;
    float wah_vocal;
    float wah_bypass;

    std::strong_ordering
    operator<=>(const ParametersData &other) const = default;
};

struct PresetData
{
    etl::string<32> name;
    ParametersData parameters;

    std::strong_ordering operator<=>(const PresetData &other) const = default;
};

struct Initialise
{
    std::strong_ordering operator<=>(const Initialise &other) const = default;
};

struct Notify
{
    id_t id;
    PresetData preset;

    std::strong_ordering operator<=>(const Notify &other) const = default;
};

struct Create
{
    PresetData preset;

    std::strong_ordering operator<=>(const Create &other) const = default;
};

struct Update
{
    id_t id;
    PresetData preset;

    std::strong_ordering operator<=>(const Update &other) const = default;
};

struct Delete
{
    id_t id;

    std::strong_ordering operator<=>(const Delete &other) const = default;
};

using PresetsApiMessage =
    std::variant<Initialise, Notify, Create, Update, Delete>;

etl::string_stream &operator<<(etl::string_stream &out,
                               const ParametersData &self);
etl::string_stream &operator<<(etl::string_stream &out, const PresetData &self);
etl::string_stream &operator<<(etl::string_stream &out, const Initialise &self);
etl::string_stream &operator<<(etl::string_stream &out, const Notify &self);
etl::string_stream &operator<<(etl::string_stream &out, const Create &self);
etl::string_stream &operator<<(etl::string_stream &out, const Update &self);
etl::string_stream &operator<<(etl::string_stream &out, const Delete &self);
etl::string_stream &operator<<(etl::string_stream &out,
                               const PresetsApiMessage &self);

} // namespace shrapnel::presets

namespace shrapnel::api {

template <>
std::optional<std::span<uint8_t>>
to_bytes(const shrapnel_presets_Preset &message, std::span<uint8_t> buffer);

template <>
std::optional<shrapnel_presets_Preset>
from_bytes(std::span<const uint8_t> buffer);

template <>
std::optional<std::span<uint8_t>>
to_bytes(const shrapnel_presets_Message &message, std::span<uint8_t> buffer);

template <>
std::optional<shrapnel_presets_Message>
from_bytes(std::span<const uint8_t> buffer);

template <>
int
to_proto(const presets::ParametersData &message, shrapnel_presets_PresetParameters &out);

template <>
std::optional<presets::ParametersData>
from_proto(const shrapnel_presets_PresetParameters &message);

template <>
int
to_proto(const presets::PresetData &message, shrapnel_presets_Preset &out);

template <>
std::optional<presets::PresetData>
from_proto(const shrapnel_presets_Preset &message);

template <>
int
to_proto(const presets::Initialise  &message, shrapnel_presets_Initialise  &out);

template <>
std::optional<presets::Initialise >
from_proto(const shrapnel_presets_Initialise &message);

template <>
int
to_proto(const presets::Notify &message, shrapnel_presets_Notify &out);

template <>
std::optional<presets::Notify>
from_proto(const shrapnel_presets_Notify &message);

template <>
int
to_proto(const presets::Create &message, shrapnel_presets_Create  &out);

template <>
std::optional<presets::Create>
from_proto(const shrapnel_presets_Create  &message);

template <>
int
to_proto(const presets::Update &message, shrapnel_presets_Update  &out);

template <>
std::optional<presets::Update>
from_proto(const shrapnel_presets_Update  &message);

template <>
int
to_proto(const presets::Delete &message, shrapnel_presets_Remove  &out);

template <>
std::optional<presets::Delete>
from_proto(const shrapnel_presets_Remove  &message);

template <>
int
to_proto(const presets::PresetsApiMessage &message, shrapnel_presets_Message  &out);

template <>
std::optional<presets::PresetsApiMessage>
from_proto(const shrapnel_presets_Message  &message);

} // namespace shrapnel::api
