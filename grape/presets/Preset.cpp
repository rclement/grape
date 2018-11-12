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

#include <grape/presets/Preset.h>

//==============================================================================

namespace grape {
namespace presets {

//==============================================================================

static const juce::String sPresetManufacturer   = juce::String::toHexString (JucePlugin_ManufacturerCode);
static const juce::String sPresetPlugin         = juce::String::toHexString (JucePlugin_PluginCode);

//==============================================================================

Preset::Preset (const juce::File& presetFile,
                const juce::String& presetBank,
                const juce::String& presetAuthor,
                const juce::String& presetComments,
                int presetVersion)
    : mFile (presetFile)
    , mName (presetFile.getFileNameWithoutExtension())
    , mBank (presetBank)
    , mAuthor (presetAuthor)
    , mComments (presetComments)
    , mVersion (presetVersion)
    , mModified (false)
    , mState ()
{

}

Preset::~Preset ()
{

}

//==============================================================================

bool Preset::operator== (const Preset& other) const
{
    return (
        mFile == other.mFile
        && mName == other.mName
        && mVersion == other.mVersion
    );
}

inline bool Preset::operator!= (const Preset& other) const
{
    return !(*this == other);
}

bool Preset::loadFromFile()
{
    if (mFile != juce::File())
    {
        juce::XmlDocument xmlDoc (mFile);
        std::unique_ptr<juce::XmlElement> xmlPreset (xmlDoc.getDocumentElement());

        if (xmlPreset.get() != nullptr && xmlPreset->hasTagName ("preset"))
        {
            const auto attManufacturer  = xmlPreset->getStringAttribute ("manufacturer");
            const auto attPlugin        = xmlPreset->getStringAttribute ("plugin");
            const auto attVersion       = xmlPreset->getIntAttribute ("version");
            const auto attAuthor        = xmlPreset->getStringAttribute ("author");
            const auto attComments      = xmlPreset->getStringAttribute ("comments");
            const auto childState       = xmlPreset->getChildByName ("state");
            const auto childInternalState = (
                childState != nullptr
                ? childState->getFirstChildElement()
                : nullptr
            );

            if (attManufacturer == sPresetManufacturer
                && attPlugin == sPresetPlugin
                && attVersion > 0
                && childInternalState != nullptr)
            {
                mVersion = attVersion;
                mModified = false;
                mAuthor = attAuthor;
                mComments = attComments;
                mState = juce::ValueTree::fromXml (*childInternalState);

                return true;
            }
        }
    }
    return false;
}

bool Preset::saveToFile()
{
    auto xmlState = new juce::XmlElement ("state");
    xmlState->addChildElement (mState.createXml());

    std::unique_ptr<juce::XmlElement> xmlPreset (new juce::XmlElement ("preset"));
    xmlPreset->setAttribute ("manufacturer",    sPresetManufacturer);
    xmlPreset->setAttribute ("plugin",          sPresetPlugin);
    xmlPreset->setAttribute ("version",         mVersion);
    xmlPreset->setAttribute ("author",          mAuthor);
    xmlPreset->setAttribute ("comments",        mComments);
    xmlPreset->addChildElement (xmlState);

    const auto parentDir = mFile.getParentDirectory();
    if (!parentDir.exists())
    {
        parentDir.createDirectory();
    }

    return xmlPreset->writeToFile (mFile, juce::String());
}

juce::XmlElement* Preset::toXml() const
{
    auto xmlState = new juce::XmlElement ("state");
    xmlState->addChildElement (mState.createXml());

    auto xml = new juce::XmlElement ("preset");
    xml->setAttribute ("file",          mFile.getFullPathName());
    xml->setAttribute ("name",          mName);
    xml->setAttribute ("bank",          mBank);
    xml->setAttribute ("author",        mAuthor);
    xml->setAttribute ("comments",      mComments);
    xml->setAttribute ("manufacturer",  sPresetManufacturer);
    xml->setAttribute ("plugin",        sPresetPlugin);
    xml->setAttribute ("version",       mVersion);
    xml->setAttribute ("modified",      mModified);
    xml->addChildElement (xmlState);

    return xml;
}

bool Preset::fromXml(const juce::XmlElement& xml)
{
    if (xml.hasTagName ("preset"))
    {
        const auto attFile 	        = xml.getStringAttribute ("file");
        const auto attName          = xml.getStringAttribute ("name");
        const auto attBank 	        = xml.getStringAttribute ("bank");
        const auto attAuthor        = xml.getStringAttribute ("author");
        const auto attComments      = xml.getStringAttribute ("comments");
        const auto attManufacturer  = xml.getStringAttribute ("manufacturer");
        const auto attPlugin        = xml.getStringAttribute ("plugin");
        const auto attVersion       = xml.getIntAttribute ("version");
        const auto attModified      = xml.getBoolAttribute ("modified");
        const auto childState       = xml.getChildByName ("state");
        const auto childInternalState = (
            childState != nullptr
            ? childState->getFirstChildElement()
            : nullptr
        );

        if (attFile != juce::String()
            && attName != juce::String()
            && attManufacturer == sPresetManufacturer
            && attPlugin == sPresetPlugin
            && attVersion > 0
            && childInternalState != nullptr)
        {
            mFile = juce::File (attFile);
            mName = attName;
            mBank = attBank;
            mAuthor = attAuthor;
            mComments = attComments;
            mVersion = attVersion;
            mModified = attModified;
            mState = juce::ValueTree::fromXml (*childInternalState);

            return true;
        }
    }

    return false;
}

juce::ValueTree Preset::copyState()
{
    return mState.createCopy();
}

void Preset::replaceState (const juce::ValueTree& newState)
{
    mState = newState;
}

bool Preset::checkState (const juce::ValueTree& newState)
{
    return mState.isEquivalentTo (newState);
}

//==============================================================================

} // namespace presets
} // namespace grape

