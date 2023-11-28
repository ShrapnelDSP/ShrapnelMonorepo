#pragma once

#include <cstdint>
#include <etl/array.h>
#include <etl/string.h>
#include <etl/string_stream.h>
#include <variant>

#include "presets.h"

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
    id_t id;
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

} // namespace shrapnel::presets