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
 *
 */

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ValvestateAudioProcessorEditor::ValvestateAudioProcessorEditor (ValvestateAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p), 
    title("titleLabel", JucePlugin_Name), 
    buttonLabel("OD1/OD2", "MODE"),
    gainLabel("gainLabel", "GAIN"), 
    bassLabel("bassLabel", "BASS"), 
    middleLabel("middleLabel", "MIDDLE"), 
    trebleLabel("trebleLabel", "TREBLE"), 
    contourLabel("contourLabel", "CONTOUR"), 
    volumeLabel("volumeLabel", "VOLUME")
{
    setResizable(false, false);
    setLookAndFeel(&vsLookAndFeel);

    applySliderStyle(gain);
    applySliderStyle(bass);
    applySliderStyle(middle);
    applySliderStyle(treble);
    applySliderStyle(contour);
    applySliderStyle(volume);

    applyLabelStyle(title);
    title.setFont(juce::Font(50));
    applyLabelStyle(gainLabel);
    applyLabelStyle(buttonLabel);
    applyLabelStyle(bassLabel);
    applyLabelStyle(middleLabel);
    applyLabelStyle(trebleLabel);
    applyLabelStyle(contourLabel);
    applyLabelStyle(volumeLabel);

    gainAttachment.reset(new SliderAttachment(processor.parameters, "gain", gain));
    bassAttachment.reset(new SliderAttachment(processor.parameters, "bass", bass));
    middleAttachment.reset(new SliderAttachment(processor.parameters, "middle", middle));
    trebleAttachment.reset(new SliderAttachment(processor.parameters, "treble", treble));
    contourAttachment.reset(new SliderAttachment(processor.parameters, "contour", contour));
    volumeAttachment.reset(new SliderAttachment(processor.parameters, "volume", volume));

    buttonAttachment.reset(new ButtonAttachment(processor.parameters, "od", button));
    button.onClick = [this] { this->repaint(); };

    addAndMakeVisible(button);
    addAndMakeVisible(gain);
    addAndMakeVisible(bass);
    addAndMakeVisible(middle);
    addAndMakeVisible(treble);
    addAndMakeVisible(contour);
    addAndMakeVisible(volume);

    addAndMakeVisible(title);
    addAndMakeVisible(buttonLabel);
    addAndMakeVisible(gainLabel);
    addAndMakeVisible(gainLabel);
    addAndMakeVisible(bassLabel);
    addAndMakeVisible(middleLabel);
    addAndMakeVisible(trebleLabel);
    addAndMakeVisible(contourLabel);
    addAndMakeVisible(volumeLabel);

    setSize (700, 200);
}

void ValvestateAudioProcessorEditor::applySliderStyle(juce::Slider &s)
{
    juce::Slider::RotaryParameters p;
    p.startAngleRadians = 2*3.1415/12*7;
    p.endAngleRadians = 2*3.1415/12*17;
    p.stopAtEnd = true;

    s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    s.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    s.setPopupDisplayEnabled(true, true, nullptr);
    s.setLookAndFeel(&vsLookAndFeel);
    s.setRotaryParameters(p);
}

void ValvestateAudioProcessorEditor::applyLabelStyle(juce::Label &l)
{
    auto f = juce::Font(20, juce::Font::FontStyleFlags::bold);
    l.setFont(f);
    l.setJustificationType(juce::Justification::centredTop);
}

ValvestateAudioProcessorEditor::~ValvestateAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}

//==============================================================================
void ValvestateAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void ValvestateAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    juce::Rectangle<int> area = getLocalBounds();

    title.setBounds(area.removeFromTop(70));

    auto knobArea = area.removeFromTop(100);
    button.setBounds(knobArea.removeFromLeft(100));
    gain.setBounds(knobArea.removeFromLeft(100));
    bass.setBounds(knobArea.removeFromLeft(100));
    middle.setBounds(knobArea.removeFromLeft(100));
    treble.setBounds(knobArea.removeFromLeft(100));
    contour.setBounds(knobArea.removeFromLeft(100));
    volume.setBounds(knobArea.removeFromLeft(100));

    buttonLabel.setBounds(area.removeFromLeft(100));
    gainLabel.setBounds(area.removeFromLeft(100));
    bassLabel.setBounds(area.removeFromLeft(100));
    middleLabel.setBounds(area.removeFromLeft(100));
    trebleLabel.setBounds(area.removeFromLeft(100));
    contourLabel.setBounds(area.removeFromLeft(100));
    volumeLabel.setBounds(area.removeFromLeft(100));
}
