#include "main.h"

#ifdef __cplusplus
extern "C" { 
#endif

#ifdef PD
FLEXT_EXT void xsample_setup()
{
	post("xsample objects, (C)2001,2002 Thomas Grill");
	post("xsample: xrecord~, xplay~, xgroove~");
	post("\tsend objects a 'help' message to get assistance");
	post("");

	xrecord_tilde_setup();
	xplay_tilde_setup();
	xgroove_tilde_setup();
}
#endif

#ifdef __cplusplus
}
#endif

// ------------------------------

V xs_obj::cb_setup(t_class *c)
{
	add_methodG(c,cb_set,"set");
	add_method0(c,cb_print,"print");
	add_method0(c,cb_refresh,"refresh");
	add_method0(c,cb_reset,"reset");
	
	add_method1(c,cb_units,"units",A_FLINT);
	add_method1(c,cb_interp,"interp",A_FLINT);
	add_method1(c,cb_sclmode,"sclmode",A_FLINT);
}

xs_obj::xs_obj():
#ifdef PD
	unitmode(xsu_sample),
#else
	unitmode(xsu_ms),
#endif
	interp(xsi_4p),
	sclmode(xss_unitsinbuf),
	curmin(0),curmax(BIGLONG)
{}
	

V xs_obj::cb_set(V *c,t_symbol *,I argc,t_atom *argv) { thisObject(c)->m_set(argc,argv); }
V xs_obj::cb_print(V *c) { thisObject(c)->m_print(); }	
V xs_obj::cb_refresh(V *c) { thisObject(c)->m_refresh(); }	
V xs_obj::cb_reset(V *c) { thisObject(c)->m_reset(); }	

V xs_obj::cb_units(V *c,FI md) { thisObject(c)->m_units((xs_unit)(I)md); }
V xs_obj::cb_interp(V *c,FI md) { thisObject(c)->m_interp((xs_intp)(I)md); }
V xs_obj::cb_sclmode(V *c,FI md) { thisObject(c)->m_sclmode((xs_sclmd)(I)md); }


I xs_obj::m_set(I argc, t_atom *argv)
{
	return buf->Set(argc >= 1?atom_getsymbolarg(0,argc,argv):NULL);
}

V xs_obj::m_refresh()
{
	buf->Set();	
	m_min(curmin); // also checks pos
	m_max(curmax); // also checks pos
}

V xs_obj::m_reset()
{
	buf->Set();
	m_min(0);
    m_max(buf->Frames()*s2u);
	m_units();
	m_sclmode();
}

V xs_obj::m_units(xs_unit mode)
{
	if(mode != xsu__) unitmode = mode;
	switch(unitmode) {
		case xsu_sample: // samples
			s2u = 1;
			break;
		case xsu_buffer: // buffer size
			s2u = 1.f/buf->Frames();
			break;
		case xsu_ms: // ms
			s2u = 1000.f/sys_getsr();
			break;
		case xsu_s: // s
			s2u = 1.f/sys_getsr();
			break;
		default:
			post("%s: Unknown unit mode",thisName());
	}
}

V xs_obj::m_interp(xs_intp mode) { interp = mode; }

V xs_obj::m_sclmode(xs_sclmd mode)
{
	if(mode != xss__) sclmode = mode;
	switch(sclmode) {
		case 0: // samples/units
			sclmin = 0; sclmul = s2u;
			break;
		case 1: // samples/units from recmin to recmax
			sclmin = curmin; sclmul = s2u;
			break;
		case 2: // unity between 0 and buffer size
			sclmin = 0; sclmul = buf->Frames()?1.f/buf->Frames():0;
			break;
		case 3:	// unity between recmin and recmax
			sclmin = curmin; sclmul = curlen?1.f/curlen:0;
			break;
		default:
			post("%s: Unknown scale mode",thisName());
	}
}

V xs_obj::m_min(F mn)
{
	mn /= s2u;  // conversion to samples
	if(mn < 0) mn = 0;
	else if(mn > curmax) mn = (F)curmax;
	curmin = (I)(mn+.5);
	curlen = curmax-curmin;

	m_sclmode();
}

V xs_obj::m_max(F mx)
{
	mx /= s2u;  // conversion to samples
	if(mx > buf->Frames()) mx = (F)buf->Frames();
	else if(mx < curmin) mx = (F)curmin;
	curmax = (I)(mx+.5);
	curlen = curmax-curmin;

	m_sclmode();
}

