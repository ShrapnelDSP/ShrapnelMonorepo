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

#include "main_thread.h"
#include "parameter_adapter.h"
#include <juce_data_structures/juce_data_structures.h>

#include "juce_core/juce_core.h"

// TODO:
// connect audio parameters to JUCE parameters
// server

// TODO does it really make sense to save the parameters as a VST plugin? That
// is the host's responsibility.
//
// We can keep saving presets, and that requires reading from parameters, but
// maybe the parameter storage code from the firmware should be removed from the
// main thread, and instead a wrapper on AudioProcessorValueTreeState should be
// injected.

// TODO this is probably unsound. The juce::Timer internal to the PropertiesFile
// will run the callback on some random thread. It locks the PropertiesFile
// lock, but setValue etc. are inherited from the PropertySet and do not lock.
// We need to lock it ourselves.
class JuceCrud final : public shrapnel::persistence::Crud<std::span<uint8_t>>
{
public:
    explicit JuceCrud(const std::string &name) : options{}
    {
        options.applicationName = name;
        options.filenameSuffix = "properties";
        options.folderName = "ShrapnelDSP";
        options.osxLibrarySubFolder = "Application Support";
        options.commonToAllUsers = false;
        options.ignoreCaseOfKeyNames = false;
        options.doNotSave = false;
        options.millisecondsBeforeSaving = 1000;
        options.storageFormat = juce::PropertiesFile::storeAsXML;
        options.processLock = nullptr;

        propertiesFile = std::make_unique<juce::PropertiesFile>(options);
    };

    int create(const std::span<uint8_t> &data, uint32_t &id_out) override
    {
        // start at 0 in case last ID has not been saved to disk
        auto id =
            static_cast<uint32_t>(propertiesFile->getIntValue(last_id_key, 0));
        id++;
        id_out = id;

        auto data_base64 = juce::Base64::toBase64(data.data(), data.size());
        propertiesFile->setValue(id_to_key(id), juce::var(data_base64));
        propertiesFile->setValue(last_id_key, juce::var(static_cast<int>(id)));

        return 0;
    }

    int read(uint32_t id, std::span<uint8_t> &data_out) override
    {
        return _read(id_to_key(id), data_out);
    }

    int update(uint32_t id, const std::span<uint8_t> &data) override
    {
        auto data_base64 = juce::Base64::toBase64(data.data(), data.size());
        propertiesFile->setValue(id_to_key(id), juce::var(data_base64));

        return 0;
    }

    int destroy(uint32_t id) override
    {
        propertiesFile->removeValue(id_to_key(id));

        return 0;
    }

    void for_each(etl::delegate<void(uint32_t, const std::span<uint8_t> &)>
                      callback) override
    {
        auto all_keys = propertiesFile->getAllProperties().getAllKeys();

        for(const auto &key : all_keys)
        {
            std::array<uint8_t, 1024> memory;
            std::span<uint8_t> buffer{memory};

            auto rc = _read(key, buffer);
            if(rc == 0)
            {
                auto id = key_to_id(key);
                callback(id, buffer);
            }
        }
    }

private:
    static juce::String id_to_key(uint32_t id)
    {
        char hex[9];
        int rc = snprintf(hex, sizeof hex, "%08" PRIx32, id);
        assert(rc == 8);
        return {hex, 9};
    };

    static uint32_t key_to_id(const juce::String &key)
    {
        id_t id;
        int rc = sscanf(key.getCharPointer(), "%08" PRIx32, &id);
        assert(rc == 1);
        return id;
    };

    int _read(juce::StringRef id, std::span<uint8_t> &data_out)
    {
        auto value = propertiesFile->getValue(id);
        if(value.isEmpty())
            return -1;

        juce::MemoryOutputStream stream;

        auto success = juce::Base64::convertFromBase64(stream, value);
        if(!success)
        {
            return -1;
        }

        if(stream.getDataSize() > data_out.size())
        {
            return -1;
        }

        std::copy(stream.getMemoryBlock().begin(),
                  stream.getMemoryBlock().end(),
                  data_out.begin());
        data_out = data_out.subspan(stream.getDataSize());

        return 0;
    }

    static constexpr char last_id_key[] = "last_id";

    juce::PropertiesFile::Options options;
    std::unique_ptr<juce::PropertiesFile> propertiesFile;
};

class JuceStorage final : public shrapnel::persistence::Storage
{
public:
    explicit JuceStorage() : options{}
    {
        options.applicationName = "storage";
        options.filenameSuffix = "properties";
        options.folderName = "ShrapnelDSP";
        options.osxLibrarySubFolder = "Application Support";
        options.commonToAllUsers = false;
        options.ignoreCaseOfKeyNames = false;
        options.doNotSave = false;
        options.millisecondsBeforeSaving = 1000;
        options.storageFormat = juce::PropertiesFile::storeAsXML;
        options.processLock = nullptr;

        propertiesFile = std::make_unique<juce::PropertiesFile>(options);
    };

    int save(const char *key, std::span<uint8_t> data) override
    {
        auto data_base64 = juce::Base64::toBase64(data.data(), data.size());
        propertiesFile->setValue(key, data_base64);
        return 0;
    }

    int save(const char *key, etl::string_view data) override
    {
        propertiesFile->setValue(key, data.data());
        return 0;
    }

    int save(const char *key, uint32_t data) override
    {
        propertiesFile->setValue(key, static_cast<int>(data));
        return 0;
    }

    int save(const char *key, float data) override
    {
        propertiesFile->setValue(key, data);
        return 0;
    }

    int load(const char *key, std::span<uint8_t> &data) override
    {
        if(!propertiesFile->containsKey(key))
        {
            return -1;
        }

        auto value = propertiesFile->getValue(key);

        juce::MemoryOutputStream stream;

        auto success = juce::Base64::convertFromBase64(stream, value);
        if(!success)
        {
            return -1;
        }

        if(stream.getDataSize() > data.size())
        {
            return -1;
        }

        std::copy(stream.getMemoryBlock().begin(),
                  stream.getMemoryBlock().end(),
                  data.begin());
        data = data.subspan(stream.getDataSize());
        return 0;
    }

    int load(const char *key, etl::istring &data) override
    {
        if(!propertiesFile->containsKey(key))
        {
            return -1;
        }

        auto value = propertiesFile->getValue(key);
        auto required_size = value.getNumBytesAsUTF8();

        data.initialize_free_space();
        data.uninitialized_resize(required_size);

        if(data.size() != required_size)
        {
            return -1;
        }

        value.copyToUTF8(data.data(), data.max_size());
        return 0;
    }

    int load(const char *key, uint32_t &data) override
    {
        if(!propertiesFile->containsKey(key))
        {
            return -1;
        }

        data = static_cast<uint32_t>(propertiesFile->getIntValue(key));
        return 0;
    }

    int load(const char *key, float &data) override
    {
        if(!propertiesFile->containsKey(key))
        {
            return -1;
        }

        data = static_cast<float>(propertiesFile->getDoubleValue(key));
        return 0;
    }

private:
    juce::PropertiesFile::Options options;
    std::unique_ptr<juce::PropertiesFile> propertiesFile;
};

class MainThread final : public juce::Thread
{
public:
    explicit MainThread(std::shared_ptr<ParameterAdapter> parameters)
        : juce::Thread{"shrapnel"},
          main_thread{
              [&](const AppMessage &message)
              {
                  //TODO print the message, later hook up to server
              },
              in_queue,
              std::move(parameters),
              std::make_shared<JuceStorage>(),
              std::make_unique<JuceCrud>("midi_mapping"),
              std::make_unique<JuceCrud>("presets"),
          }
    {
    }

    void run() override
    {
        juce::Logger::writeToLog("Main thread started");

        while(!threadShouldExit())
        {
            wait(10);

            main_thread.loop();
        }
    };

private:
    // A queue filled by the server with received API messages
    shrapnel::Queue<AppMessage, 4> in_queue;
    shrapnel::MainThread<4, ParameterAdapter> main_thread;
};
