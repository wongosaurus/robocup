/*! @file Behaviour.cpp
    @brief Implementation of behaviour class

    @author Jason Kulk
 
 Copyright (c) 2009 Jason Kulk
 
 This file is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 This file is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with NUbot.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "targetconfig.h"


#include "Behaviour.h"
#include "BehaviourProvider.h"

#include "MiscBehaviours/SelectBehaviourProvider.h"
#include "Soccer/SoccerProvider.h"
#include "MiscBehaviours/VisionCalibrationProvider.h"
#include "ChaseBall/ChaseBallProvider.h"
#include "WalkOptimisation/WalkOptimisationProvider.h"
#include "Kicker/KickerProvider.h"
#include "PassingChallenge/PassingChallengeProvider.h"
#include "MiscBehaviours/PoseProvider.h"
#include "MiscBehaviours/ScriptedPoseProvider.h"
#include "MiscBehaviours/ForwardWalkProvider.h"
#include "RoboPedestrian/RoboPedestrianProvider.h"

#include "CameraCalibration/CameraCalibrationProvider.h"
#include "EnvironmentalEmotions/EnvironmentalEmotionsProvider.h"


#ifdef TARGET_IS_BEAR
    #include "BearMode/BearModeProvider.h"
#endif
#ifdef TARGET_IS_CYCLOID
    #include "Cycloid/QuietStance/QuietStanceProvider.h"
#endif

#include "debug.h"
#include "debugverbositybehaviour.h"
using namespace std;

Behaviour::Behaviour()
{
    #if defined(TARGET_IS_NAOWEBOTS)
        m_behaviour = new SoccerProvider(this);
    #elif defined(TARGET_IS_BEAR)
        m_behaviour = new BearModeProvider(this);
    #elif defined(TARGET_IS_CYCLOID)
        m_behaviour = new QuietStanceProvider(this);
    #else
        m_behaviour = new SoccerProvider(this);
    #endif
    m_next_behaviour = NULL;
}

Behaviour::~Behaviour()
{
    delete m_behaviour;
    if (m_next_behaviour != NULL)
        delete m_next_behaviour;
}

/*! @brief The main behaviour process function.
        
    Calls the process function of the current behaviour provider and handles change of provider when there is a next one.

    @param jobs the nubot job list
    @param data the nubot sensor data
    @param actions the nubot actionators data
    @param fieldobjects the nubot world model
    @param gameinfo the nubot game information
    @param teaminfo the nubot team information
*/
void Behaviour::process(JobList* jobs, NUSensorsData* data, NUActionatorsData* actions, FieldObjects* fieldobjects, GameInformation* gameinfo, TeamInformation* teaminfo)
{
    #if DEBUG_BEHAVIOUR_VERBOSITY > 0
        debug << "Behaviour::process()" << endl;
    #endif
    if (m_next_behaviour != NULL)
    {
        #if DEBUG_BEHAVIOUR_VERBOSITY > 0
            debug << "Behaviour::process() is swaping the behaviour provider" << endl;
        #endif
        delete m_behaviour;
        m_behaviour = m_next_behaviour;
        m_next_behaviour = NULL;
    }
    m_behaviour->process(jobs, data, actions, fieldobjects, gameinfo, teaminfo);
}

void Behaviour::setNextBehaviour(std::string name)
{
    m_next_behaviour = nameToProvider(name);
}

void Behaviour::setNextBehaviour(BehaviourProvider* behaviour)
{
    m_next_behaviour = behaviour;
}


BehaviourProvider* Behaviour::nameToProvider(std::string name)
{
    name = simplifyName(name);
    if (name.compare("selectbehaviour") == 0)
        return new SelectBehaviourProvider(this);
    else if (name.find("soccer") != string::npos)
        return new SoccerProvider(this);
    else if (name.compare("chaseball") == 0)
        return new ChaseBallProvider(this);
    else if (name.compare("visioncalibration") == 0 or name.find("saveimage") != string::npos)
        return new VisionCalibrationProvider(this);
    else if (name.find("walkoptimis") != string::npos)
        return new WalkOptimisationProvider(this);
    else if (name.find("kicker") != string::npos)
        return new KickerProvider(this);
    else if (name.compare("scriptedpose") == 0)
        return new ScriptedPoseProvider(this);
    else if (name.compare("pose") == 0)
        return new PoseProvider(this);
    else if (name.compare("robopedestrian") == 0)
        return new RoboPedestrianProvider(this);
	else if (name.compare("cameracalibration") == 0)
        return new CameraCalibrationProvider(this);
    else if (name.find("enviro") != string::npos)
        return new EnvironmentalEmotionsProvider(this);
    else
        return NULL;
}


/*! @brief Simplifies a name. The name is converted to lowercase, and spaces, underscores, forward slash, backward slash and dots are removed from the name.
    @param input the name to be simplified
    @return the simplified string
 */
string Behaviour::simplifyName(const string& input)
{
    string namebuffer, currentletter;
    // compare each letter to a space and an underscore and a forward slash
    for (unsigned int j=0; j<input.size(); j++)
    {
        currentletter = input.substr(j, 1);
        if (currentletter.compare(string(" ")) != 0 && currentletter.compare(string("_")) != 0 && currentletter.compare(string("/")) != 0 && currentletter.compare(string("\\")) != 0 && currentletter.compare(string(".")) != 0)
            namebuffer += tolower(currentletter[0]);            
    }
    return namebuffer;
}
