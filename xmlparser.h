#ifndef _XMLPARSER_H_
#define _XMLPARSER_H_

#include <string>

#include <libxml/tree.h>
#include <libxml/parser.h>

#include "duserclient.h"
#include "gamestructs.h"
#include "gameutils.h"
#include "gamecampaigndefinition.h"
//#include "gamecar.h"

namespace XmlParser {

	DUserClientConfig parseConfig(std::string filename);

	GameCarBody* parseCar(std::string filename);
	GameTrackDesc* parseTrack(std::string filename);
	GameLevelDesc* parseLevel(std::string filename);
	GameCampaignDefinition* parseCampaign(std::string filename);
}

#endif
