#pragma once

namespace ege
{

class PushTarget {
public:
	PushTarget() { m_target = gettarget(); }
	PushTarget(PIMAGE target) { m_target = gettarget(); settarget(target); }
	~PushTarget() { settarget(m_target); }
private:
	PIMAGE m_target;
};

}

