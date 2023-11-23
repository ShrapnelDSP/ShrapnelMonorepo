// TODO
// JSON encoders
// JSON decoders

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
    float ampGain;
    float ampChannel;
    float bass;
    float middle;
    float treble;
    float contour;
    float volume;
    float noiseGateThreshold;
    float noiseGateHysteresis;
    float noiseGateAttack;
    float noiseGateHold;
    float noiseGateRelease;
    float noiseGateBypass;
    float chorusRate;
    float chorusDepth;
    float chorusMix;
    float chorusBypass;
    float wahPosition;
    float wahVocal;
    float wahBypass;

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
};

struct Notify
{
    PresetData preset;
};

struct Create
{
    PresetData preset;
};

struct Update
{
    PresetData preset;
};

struct Delete
{
    id_t id;
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