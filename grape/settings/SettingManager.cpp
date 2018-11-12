/*
    GRAPE is Romain's Audio Plug-in Extension classes for the JUCE framework

    Copyright (c) 2018 Romain Clement

    MIT License    
    
    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:
    
    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.
    
    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
 */

//==============================================================================

#include <grape/settings/SettingManager.h>

//==============================================================================

namespace grape {
namespace settings {

//==============================================================================

SettingManager::SettingManager (const std::vector<Setting>& settingsInfo,
                                juce::UndoManager* undoManager,
                                const juce::String& identifier)
    : mSettingsInfo (settingsInfo)
    , mUndoManager (undoManager)
{
    mSettings = juce::ValueTree (juce::Identifier (identifier));

    for (const auto& s : mSettingsInfo)
    {
        mSettings.setProperty (
            juce::Identifier (s.id),
            juce::var (s.defaultValue),
            mUndoManager
        );
    }

    mSettings.addListener (this);
}

SettingManager::~SettingManager()
{
    mSettings.removeListener (this);
}

//==============================================================================

juce::var SettingManager::getSetting (const juce::String& identifier)
{
    return mSettings.getProperty (juce::Identifier (identifier));
}

void SettingManager::setSetting (const juce::String& identifier, const juce::var& value)
{
    mSettings.setProperty (juce::Identifier (identifier), value, mUndoManager);
}

juce::XmlElement* SettingManager::toXml()
{
    const auto stateSettings = mSettings.createCopy();
    return stateSettings.createXml();
}

void SettingManager::fromXml (const juce::XmlElement& xmlState)
{
    if (xmlState.hasTagName (mSettings.getType()))
    {
        mSettings = juce::ValueTree::fromXml (xmlState);
    }
}

void SettingManager::addListener (Listener* listener)
{
    mListeners.add (listener);
}

void SettingManager::removeListener (Listener* listener)
{
    mListeners.remove (listener);
}

//==============================================================================

void SettingManager::notifySettingChanged (const juce::String& identifier, const juce::var& value)
{
    mListeners.call (
        [&] (Listener& l) { l.settingChanged (identifier, value); }
    );
}

//==============================================================================

void SettingManager::valueTreePropertyChanged (juce::ValueTree& tree, const juce::Identifier& identifier)
{
    const auto id = identifier.toString();
    const auto value = mSettings.getProperty (identifier);
    notifySettingChanged (id, value);
}

void SettingManager::valueTreeChildAdded (juce::ValueTree&, juce::ValueTree&)
{

}

void SettingManager::valueTreeChildRemoved (juce::ValueTree&, juce::ValueTree&, int)
{

}

void SettingManager::valueTreeChildOrderChanged (juce::ValueTree&, int, int)
{

}

void SettingManager::valueTreeParentChanged (juce::ValueTree&)
{

}

void SettingManager::valueTreeRedirected (juce::ValueTree& tree)
{
    for (int i = 0; i < tree.getNumProperties(); ++i)
    {
        const auto name = tree.getPropertyName (i);
        const auto prop = tree.getProperty (juce::Identifier (name));
        notifySettingChanged (name.toString(), prop);
    }
}

//==============================================================================

} // namespace settings
} // namespace grape

