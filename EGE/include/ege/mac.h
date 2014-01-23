#ifndef Inc_ege_mac_h_
#define Inc_ege_mac_h_

#define CTL_PREINIT(classname, parent) struct preinit_obj \
	{ \
		preinit_obj(classname* This, int inheritlevel) \
		{ \
			This->pre_init(inheritlevel); \
		} \
	}_preinit_obj; \
	enum inherit_e \
	{ \
		inherit_level_e = parent::inherit_level_e + 1 \
	}; \
	static void firstinit(ege::egeControlBase* This) \
	{ \
		((classname*)This)->m_inheritlevel = 1; \
	} \
	void pre_init(int inheritlevel) \
	{ \

#define CTL_PREINITEND  }
#define CTL_DEFPARAM    int inherit = inherit_level_e, ege::egeControlBase* pParent = {}
#define CTL_INITBASE(parent)    parent(inherit, (ege::egeControlBase*)pParent), _preinit_obj(this, inherit_level_e)
#define CTL_INIT        InitObject iobj(this, inherit_level_e);\
	ege::PushTarget _pushtarget(buf());

#endif

