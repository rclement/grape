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

#include <grape/parameters/ParameterManager.h>

//==============================================================================

namespace grape {
namespace parameters {

//==============================================================================

ParameterManager::ParameterManager (juce::AudioProcessor& processor,
                                    juce::UndoManager* undoManager,
                                    const std::vector<grape::parameters::Parameter>& parametersInfo,
                                    const juce::String& identifier)
    : AudioProcessorValueTreeState (processor, undoManager)
    , mParametersInfo (parametersInfo)
{
    for (const auto& p : mParametersInfo)
    {
        addParameter (p);
    }

    state = juce::ValueTree (juce::Identifier (identifier));
}

ParameterManager::~ParameterManager()
{

}

//==============================================================================

void ParameterManager::resetAll()
{
    for (const auto& p : mParametersInfo)
    {
        auto param = getParameter (p.id);
        jassert (param != nullptr);

        const auto defaultValue = param->getDefaultValue();
        param->setValueNotifyingHost (defaultValue);
    }
}

juce::XmlElement* ParameterManager::toXml()
{
    const auto stateParameters = copyState();
    return stateParameters.createXml();
}

void ParameterManager::fromXml (const juce::XmlElement& xmlState)
{
    if (xmlState.hasTagName (state.getType()))
    {
        replaceState (juce::ValueTree::fromXml (xmlState));
    }
}

//==============================================================================

void ParameterManager::addParameter (const parameters::Parameter& parameter)
{
    createAndAddParameter (
        parameter.id,
        parameter.name,
        parameter.label,
        parameter.valueRange,
        parameter.defaultValue,
        parameter.valueToTextFunction,
        parameter.textToValueFunction,
        parameter.isMeta,
        parameter.isAutomatable,
        parameter.isDiscrete,
        parameter.category,
        parameter.isBoolean
    );
}

//==============================================================================

} // namespace parameters
} // namespace grape

