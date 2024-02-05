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

#include "juce_core/juce_core.h"

// TODO:
// connect audio parameters to JUCE parameters
// server

// TODO implement based on https://docs.juce.com/master/classPropertiesFile.html
class JuceCrud final : public shrapnel::persistence::Crud<std::span<uint8_t>>
{
    
};

// TODO implement based on https://docs.juce.com/master/classPropertiesFile.html
class JuceStorage final : public shrapnel::persistence::Storage
{
    
};

class MainThread final : public juce::Thread
{
public:
    MainThread()
        : juce::Thread("shrapnel"),
          main_thread(
              [&](const AppMessage &message)
              {
                  //TODO print the message, later hook up to server
              },
              in_queue,
              audio_parameters,
              std::make_shared<JuceStorage>(),
              std::make_unique<JuceCrud>("midi_mapping"),
              std::make_unique<JuceCrud>("presets"))
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
    shrapnel::MainThread<20, 4> main_thread;
};
