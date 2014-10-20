#include "luaapi.h"


#define LUA_PROP_INTERFACE_DEFS(objType) \
	    .def("getPropInt",&objType::getPropInt) \
	    .def("setPropInt",&objType::setPropInt) \
	    .def("getPropFloat",&objType::getPropFloat) \
	    .def("setPropFloat",&objType::setPropFloat) \
	    .def("getPropString",&objType::getPropString) \
	    .def("setPropString",&objType::setPropString)

void luaapi_bind(lua_State* lua) {

	luabind::module(lua) [
		luabind::class_<LuaGameCar>("GameCar")
		.def("turn",&LuaGameCar::turn)
		.def("accelerate",&LuaGameCar::accelerate)
		.def("brake",&LuaGameCar::brake)
		.def("getPos",&LuaGameCar::getPos)
		.def("getSpeed",&LuaGameCar::getSpeed)
		.def("getSpeedVect",&LuaGameCar::getSpeedVect)
		.def("getHeadingVect",&LuaGameCar::getHeadingVect)
		.def("sensorsInit",&LuaGameCar::sensorsInit)
		.def("getSensorData",&LuaGameCar::getSensorData)
		.def("getSensorAngle",&LuaGameCar::getSensorAngle)
		.def("getSensorPixels",&LuaGameCar::getSensorPixels)
		.def("getSensorEnabled",&LuaGameCar::getSensorEnabled)
		.def("getLap",&LuaGameCar::getLap)
		.def("getScore",&LuaGameCar::getScore)
		.def("setScore",&LuaGameCar::setScore)
		.def("getRank",&LuaGameCar::getRank)
		.def("getFinalScore",&LuaGameCar::getFinalScore)
		.def("setFinalScore",&LuaGameCar::setFinalScore)
		.def("isDone",&LuaGameCar::isDone)
		.def("getRoadClosest",&LuaGameCar::getRoadClosest)
	];

	luabind::module(lua) [
   	    luabind::class_<LuaRenderNode>("RenderNode")
   	    .def("setPos",&LuaRenderNode::setPos)
		.def("setAngle",&LuaRenderNode::setAngle)
		.def("setScale",&LuaRenderNode::setScale)
		.def("setVisible",&LuaRenderNode::setVisible)
		.def("setObject",&LuaRenderNode::setObject)
		.def("createNode",&LuaRenderNode::createNode)
	];

	luabind::module(lua) [
	    luabind::class_<LuaUserClient>("UserClient")
	    .def("setAmbient",&LuaUserClient::setAmbient)
	    .def("setSunDir",&LuaUserClient::setSunDir)
	    .def("setSunColor",&LuaUserClient::setSunColor)
	    .def("color",&LuaUserClient::color)
	    .def("moveTo",&LuaUserClient::moveTo)
	    .def("lineTo",&LuaUserClient::lineTo)
	    .def("saveMatrix",&LuaUserClient::saveMatrix)
	    .def("restoreMatrix",&LuaUserClient::restoreMatrix)
	    .def("transform",&LuaUserClient::transform)
	    .def("text",&LuaUserClient::text)
	    .def("getMousePos",&LuaUserClient::getMousePos)
	    .def("getMousePressed",&LuaUserClient::getMousePressed)
	    .def("getMouseDown",&LuaUserClient::getMouseDown)
	    .def("createObject",&LuaUserClient::createObject)
	];

	luabind::module(lua) [
	    luabind::class_<LuaUserClientShader>("UserClientShader")
	    .def(luabind::constructor<>())
	    .def("compile",&LuaUserClientShader::compile)
	    .def("compileFromFile",&LuaUserClientShader::compileFromFile)
	    .def("free",&LuaUserClientShader::free)
	    .def("getUniform",&LuaUserClientShader::getUniform)
    ];

	luabind::module(lua) [
	    luabind::class_<LuaUserClientObject>("UserClientObject")
	    .def("setShader",&LuaUserClientObject::setShader)
	    .def("setFloat",&LuaUserClientObject::setFloat)
	    .def("addTexture",&LuaUserClientObject::addTexture)
	    .def("setTexture",&LuaUserClientObject::setTexture)
    ];

	luabind::module(lua) [
	    luabind::class_<DResource>("Resource")
    ];
	luabind::module(lua) [
		luabind::class_<DUserClientObject>("RenderObject")
    ];

	luabind::module(lua) [
        luabind::class_<GameRoadGeometryMetapoint>("GameRoadGeometryMetapoint")
        .def_readwrite("center",&GameRoadGeometryMetapoint::center)
        .def_readwrite("left",&GameRoadGeometryMetapoint::left)
        .def_readwrite("local_pos",&GameRoadGeometryMetapoint::local_pos)
    ];

	luabind::module(lua) [
        luabind::class_<LuaGameRoad>("GameRoad")
        .def("getMeta",&LuaGameRoad::getMeta)
        .def("getClosest",&LuaGameRoad::getClosest)
        .def("getLength",&LuaGameRoad::getLength)
        .def("isCyclic",&LuaGameRoad::isCyclic)
    ];

	luabind::module(lua) [
        luabind::class_<LuaGameTrack>("GameTrack")
        .def("getRoadSize",&LuaGameTrack::getRoadSize)
        .def("getRoad",&LuaGameTrack::getRoad)
        .def("surfaceQuery",&LuaGameTrack::surfaceQuery)
    ];

	luabind::module(lua) [
        luabind::class_<LuaGameLevel>("GameLevel")
        .def("addCar",&LuaGameLevel::addCar)
        .def("addObject", (void(LuaGameLevel::*)(string,float,float,float)) &LuaGameLevel::addObject)
        .def("addObject", (void(LuaGameLevel::*)(string,float,float,float,bool)) &LuaGameLevel::addObject)
        .def("addObject", (void(LuaGameLevel::*)(string,float,float,float,bool,float)) &LuaGameLevel::addObject)
        .def("getTrack",&LuaGameLevel::getTrack)
        .def("lockCars",&LuaGameLevel::lockCars)
        .def("releaseCars",&LuaGameLevel::releaseCars)
        .def("getCarSize",&LuaGameLevel::getCarSize)
        .def("getCar",&LuaGameLevel::getCar)

        LUA_PROP_INTERFACE_DEFS(LuaGameLevel)
    ];

	luabind::module(lua) [
        luabind::class_<dvect>("vec2")
        .def(luabind::constructor<dvtype,dvtype>())
        .def("len",&dvect::length)
        .def("len_fast",&dvect::length_fast)
        .def("unrotate",&dvect::unrotate)
        .def_readwrite("x",&dvect::x)
        .def_readwrite("y",&dvect::y)

        .def(luabind::self * float())
        .def(luabind::self + luabind::other<dvect>())
        .def(luabind::self - luabind::other<dvect>())
	];

	luabind::module(lua) [
        luabind::class_<dvect3>("vec3")
        .def(luabind::constructor<dvtype,dvtype,dvtype>())
        .def_readwrite("x",&dvect3::x)
        .def_readwrite("y",&dvect3::y)
        .def_readwrite("z",&dvect3::z)
	];

	luabind::module(lua) [
	    luabind::class_<LuaGameResourceManager>("GameResourceManager")
	    .def("getFont",&LuaGameResourceManager::getFont)
	    .def("getImg",&LuaGameResourceManager::getImg)
    ];

	luabind::module(lua) [
	    luabind::class_<LuaGameMenu>("GameMenu")
	    .def("close",&LuaGameMenu::close)
	    .def("show",&LuaGameMenu::show)
	    .def("getRootNode",&LuaGameMenu::getRootNode)

	    LUA_PROP_INTERFACE_DEFS(LuaGameMenu)
    ];
	luabind::module(lua) [
	    luabind::class_<LuaGameWidget>("GameWidget")
	    .def("mouseClicked",&LuaGameWidget::mouseClicked)
	    .def("mouseDown",&LuaGameWidget::mouseDown)
	    .def("mouseInside",&LuaGameWidget::mouseInside)
	    .def("mousePos",&LuaGameWidget::mousePos)
	    .def("getPos",&LuaGameWidget::getPos)
	    .def("getSize",&LuaGameWidget::getSize)
	    .def("show",&LuaGameWidget::show)

	    LUA_PROP_INTERFACE_DEFS(LuaGameWidget)
    ];

	luabind::module(lua) [
	    luabind::class_<LuaGameCampaign>("GameCampaign")
	    .def("countCars", &LuaGameCampaign::countCars)
	    .def("getPlayerName", &LuaGameCampaign::getPlayerName)
	    .def("setPlayerName", &LuaGameCampaign::setPlayerName)
	    .def("getBudget", &LuaGameCampaign::getBudget)
	    .def("setBudget", &LuaGameCampaign::setBudget)
	    .def("subtractBudget", &LuaGameCampaign::subtractBudget)
	    .def("addCar", &LuaGameCampaign::addCar)
	    .def("getCars", &LuaGameCampaign::getCars)
	    .def("isLevelUnlocked", &LuaGameCampaign::isLevelUnlocked)
	    .def("setLevelUnlocked", &LuaGameCampaign::setLevelUnlocked)
	    ];

	luabind::module(lua) [
	    luabind::class_<LuaGameCampaignDefinition>("GameCampaignDefinition")
	    .def("addLevel", &LuaGameCampaignDefinition::addLevel)
	    .def("getLevels", &LuaGameCampaignDefinition::getLevels)
	    ];

	luabind::module(lua) [
	    luabind::class_<LuaGameEntityTree>("GameEntityTree")
	    .def("getEntity", &LuaGameEntityTree::getEntity)
	    .def("addEntity", &LuaGameEntityTree::addEntity)
	    .def("getFirst", &LuaGameEntityTree::getFirst)
	    ];

	luabind::module(lua) [
		luabind::class_<LuaGameEntity>("GameEntity")
		.def("getLightestChild", &LuaGameEntity::getLightestChild)
		.def("getHeaviestChild", &LuaGameEntity::getHeaviestChild)
		.def("addChild", &LuaGameEntity::addChild)
		.def("removeChild", &LuaGameEntity::removeChild)
		.def("getScore", &LuaGameEntity::getScore)
		.def("setScore", &LuaGameEntity::setScore)
		.def("addCar", &LuaGameEntity::addCar)
		.def("getCars", &LuaGameEntity::getCars)
		];
	//======================================
	luabind::module(lua) [
		luabind::class_<LuaGameEntityChild>("GameEntityChild")
		.def("getId", &LuaGameEntityChild::getId)
		.def("setId", &LuaGameEntityChild::setId)
		.def("getWeight", &LuaGameEntityChild::getWeight)
		.def("setWeight", &LuaGameEntityChild::setWeight)
		];

	luabind::module(lua) [
			luabind::class_<LuaGameTweaker>("GameTweaker")
	        .def(luabind::constructor<>())
			.def("setTyreOutForceMult", &LuaGameTweaker::setTyreOutForceMult)
			.def("setTyreGripForceMult", &LuaGameTweaker::setTyreGripForceMult)
			.def("setTyreSlideTreshold", &LuaGameTweaker::setTyreSlideTreshold)
		];

	cout<<"luaapi bound"<<endl;
}



