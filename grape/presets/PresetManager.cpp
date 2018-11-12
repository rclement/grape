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

#include <grape/presets/PresetManager.h>

//==============================================================================

namespace grape {
namespace presets {

//==============================================================================

static bool isMacOS ()
{
    static const bool macOS = (
        (juce::SystemStats::getOperatingSystemType()
         & juce::SystemStats::OperatingSystemType::MacOSX) != 0
    );

    return macOS;
}

//==============================================================================

PresetManager::PresetManager (parameters::ParameterManager& parameters)
    : mParameters (parameters)
    , mPresetChecker (*this)
{
    loadDefaultPreset();
}

PresetManager::~PresetManager()
{

}

//==============================================================================

juce::File PresetManager::getFactoryPresetsLocation() const
{
    static const auto location = juce::File::getSpecialLocation (
        juce::File::SpecialLocationType::commonApplicationDataDirectory
    )
    .getChildFile (isMacOS() ? "Application Support" : "")
    .getChildFile (JucePlugin_Manufacturer)
    .getChildFile (JucePlugin_Name)
    .getChildFile ("presets");

    return location;
}

juce::File PresetManager::getUserPresetsLocation() const
{
    static const auto location = juce::File::getSpecialLocation (
        juce::File::SpecialLocationType::userApplicationDataDirectory
    )
    .getChildFile (isMacOS() ? "Application Support" : "")
    .getChildFile (JucePlugin_Manufacturer)
    .getChildFile (JucePlugin_Name)
    .getChildFile ("presets");

    if (!location.exists())
        location.createDirectory();

    return location;
}

juce::Array<Preset> PresetManager::getFactoryPresets() const
{
    return findPresets (getFactoryPresetsLocation());
}

juce::Array<Preset> PresetManager::getUserPresets() const
{
    return findPresets (getUserPresetsLocation());
}

juce::Array<Preset> PresetManager::getAllPresets() const
{
    auto allPresets = getFactoryPresets();
    allPresets.addArray (getUserPresets());
    return allPresets;
}

Preset PresetManager::getFactoryPreset (const juce::String& presetName,
                                        const juce::String& presetBank) const
{
    const auto factoryPresets = getFactoryPresets();
    for (auto& p : factoryPresets)
    {
        if (p.getName() == presetName && p.getBank() == presetBank)
        {
            return p;
        }
    }

    if (presetName == juce::String())
        return Preset();

    const auto factoryLocation = getFactoryPresetsLocation();
    const auto factoryPresetFile = (
        factoryLocation
            .getChildFile (presetBank)
            .getChildFile (presetName)
            .withFileExtension ("xml")
    );

    const auto bank = findPresetBank (factoryPresetFile, factoryLocation);
    return Preset (factoryPresetFile);
}

Preset PresetManager::getUserPreset (const juce::String& presetName,
                                     const juce::String& presetBank) const
{
    const auto userPresets = getUserPresets();
    for (auto& p : userPresets)
    {
        if (p.getName() == presetName && p.getBank() == presetBank)
        {
            return p;
        }
    }

    if (presetName == juce::String())
        return Preset();

    const auto userLocation = getUserPresetsLocation();
    const auto userPresetFile = (
        userLocation
            .getChildFile (presetBank)
            .getChildFile (presetName)
            .withFileExtension ("xml")
    );

    const auto bank = findPresetBank (userPresetFile, userLocation);
    return Preset (userPresetFile, bank);
}

Preset PresetManager::getCurrentPreset() const
{
    return mCurrentPreset;
}

void PresetManager::loadDefaultPreset()
{
    mParameters.resetAll();

    mCurrentPreset = Preset();
    mCurrentPreset.setName ("Default");
    mCurrentPreset.replaceState (mParameters.copyState());
    findCurrentPresetIndex();

    notifyPresetChanged();
}

void PresetManager::loadPreset (const Preset& preset)
{
    auto loadedPreset = preset;
    if (loadedPreset.loadFromFile())
    {
        mCurrentPreset = loadedPreset;
        mParameters.replaceState (mCurrentPreset.copyState());
        findCurrentPresetIndex();
        notifyPresetChanged();
    }
}

void PresetManager::loadPreviousPreset()
{
    loadPresetAtIndex (mCurrentPresetIndex - 1);
}

void PresetManager::loadNextPreset()
{
    loadPresetAtIndex (mCurrentPresetIndex + 1);
}

bool PresetManager::canLoadPreviousPreset()
{
    return mCurrentPresetIndex > 0;
}

bool PresetManager::canLoadNextPreset()
{
    const auto allPresets = getAllPresets();
    return mCurrentPresetIndex < (allPresets.size() - 1);
}

bool PresetManager::saveCurrentPreset (const juce::String& presetName,
                                       const juce::String& presetBank,
                                       const juce::String& presetAuthor,
                                       const juce::String& presetComments)
{
    auto userPreset = getUserPreset (presetName, presetBank);
    userPreset.setAuthor (presetAuthor);
    userPreset.setComments (presetComments);
    userPreset.replaceState (mParameters.copyState());

    if (userPreset.saveToFile())
    {
        loadPreset (userPreset);
        return true;
    }

    return false;
}

juce::XmlElement* PresetManager::toXml()
{
    const auto xmlCurrentPreset = mCurrentPreset.toXml();

    auto xmlState = new juce::XmlElement ("preset-manager");
    xmlState->insertChildElement (xmlCurrentPreset, 0);

    return xmlState;
}

void PresetManager::fromXml (const juce::XmlElement& xmlState)
{
    if (xmlState.hasTagName ("preset-manager"))
    {
        const auto xmlPreset = xmlState.getChildElement (0);
        if (xmlPreset != nullptr && mCurrentPreset.fromXml (*xmlPreset))
        {
            findCurrentPresetIndex();
            notifyPresetChanged();
        }
    }
}

void PresetManager::checkPresetChanged ()
{
    const auto stateParameters = mParameters.copyState();
    const auto different = !mCurrentPreset.checkState (stateParameters);
    const auto modified = mCurrentPreset.isModified();

    if (modified != different)
    {
        mCurrentPreset.setModified (different);
        notifyPresetChanged();
    }
}

void PresetManager::addListener (Listener* listener)
{
    mListeners.add (listener);
}

void PresetManager::removeListener (Listener* listener)
{
    mListeners.remove (listener);
}

//==============================================================================

juce::Array<Preset> PresetManager::findPresets (const juce::File& location) const
{
    const auto presetFiles = location.findChildFiles (
        juce::File::TypesOfFileToFind::findFiles, true, "*.xml"
    );

    juce::Array<Preset> presetsList;
    for (auto& f : presetFiles)
    {
        const auto bank = findPresetBank (f, location);
        presetsList.add (Preset (f, bank));
    }
    return presetsList;
}

juce::String PresetManager::findPresetBank (const juce::File& presetFile,
                                            const juce::File& presetBaseLocation) const
{
    auto bank = presetFile.getParentDirectory().getRelativePathFrom (presetBaseLocation);

    if (bank == ".")
        return juce::String();

    return bank.upToFirstOccurrenceOf ("/", false, false);
}

void PresetManager::notifyPresetChanged ()
{
    mListeners.call (
        [&] (Listener& l) { l.presetChanged (mCurrentPreset); }
    );
}

void PresetManager::findCurrentPresetIndex()
{
    const auto allPresets = getAllPresets();

    mCurrentPresetIndex = -1;
    for (int i = 0; i < allPresets.size(); ++i)
    {
        const auto& p = allPresets[i];
        if (p == mCurrentPreset)
        {
            mCurrentPresetIndex = i;
            return;
        }
    }
}

void PresetManager::loadPresetAtIndex (int presetIndex)
{
    const auto allPresets = getAllPresets();
    const auto numPresets = allPresets.size();
    const auto newPresetIndex = juce::jmin (juce::jmax (presetIndex, 0), numPresets - 1);
    loadPreset (allPresets[newPresetIndex]);
}

//==============================================================================

} // namespace presets
} // namespace grape

