

#pragma once

#include <JuceHeader.h>

namespace juced
{

class PushButton : public juce::TextButton
{
public:
    enum ColourIds
    {
        textColourOffId = 0x1000102,  
        textColourOnId = 0x1000103,
        frameColourId = 0x1000104,
        backgroundColourOnId = 0x1000105,
        backgroundColourOffId = 0x1000106
    };

    PushButton (juce::String name, juce::String label)
        : TextButton (name)
    {
        setColour (textColourOffId, juce::Colours::white);
        setColour (textColourOnId, juce::Colour::fromRGBA(66, 162, 200, 255));
        setColour (frameColourId, juce::Colour::fromRGBA(66, 162, 200, 255));
        setColour (backgroundColourOffId, juce::Colour::fromRGBA(37, 49, 55, 255));
        setColour (backgroundColourOnId, juce::Colour::fromRGBA(23, 30, 34, 255));
        setButtonText (label);
        setClickingTogglesState (true);
    }

    void paintButton (juce::Graphics& g,
                      bool shouldDrawButtonAsHighlighted,
                      bool shouldDrawButtonAsDown) override
    {
        auto& lf = getLookAndFeel();
        auto b = getLocalBounds().toFloat().reduced (4, 4);
        auto pushState = (getToggleState() ? 0.0 : -2.0);
        auto cornerSize = b.getHeight() * 0.5;
        g.setColour (findColour (frameColourId));
        g.drawRoundedRectangle (b, cornerSize, 2.0f);
        g.setColour (findColour (getToggleState() ? backgroundColourOnId : backgroundColourOffId));
        g.fillRoundedRectangle (b, cornerSize + pushState);
        lf.drawButtonText (g, *this, shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown);
    }

    void resized() override
    {
        TextButton::resized();
    }

private:

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PushButton)
};

} // namespace juced
