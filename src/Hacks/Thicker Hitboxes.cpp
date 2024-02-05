#include <Geode/Geode.hpp>
#include "../Client/Client.h"

using namespace geode::prelude;

bool myDrawPoly(CCDrawNode* ins, CCPoint *verts, unsigned int count, const ccColor4F &fillColor, float borderWidth, const ccColor4F &borderColor) {
    return ins->drawPolygon(verts, count, fillColor, borderWidth * (((ins->getTag() == -9999) && Client::GetModuleEnabled("thick-hitbox")) ? 2.2f /*2.2?! HOLY SHIT IS THAT A MOTHERFUCKING GEOMETRY DASH REFERENCE*/ : 1), borderColor);
}

$execute {
    Mod::get()->hook(
        reinterpret_cast<void*>(
            geode::addresser::getNonVirtual(&CCDrawNode::drawPolygon)
        ),
        &myDrawPoly,
        "cocos2d::CCDrawNode::drawPolygon",
        tulip::hook::TulipConvention::Thiscall
    );
}