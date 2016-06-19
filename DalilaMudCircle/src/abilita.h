/**************************************************************************
 *        abilita.h - Tabella dei modificatori derivanti dalle abilita    *
 **************************************************************************/

//Qui cmnq ci sono solo i modificatori :doppio danno ,paralize,
//spellfail per Maglia e Piastre non ci sono.
//Nel senso che sono calcolati in modo diverso.

#define NONDECAPITATO	0
#define DECAPITATO	1
#define OBJVNUM_TESTA	43

float abil_armatura (struct char_data *ch, int attacktype);
int abil_scudo (struct char_data *ch, struct char_data *vict);
int check_abil_spada (struct char_data *ch);

ACMD(do_catarsi);
ACMD(do_rigenerazione);

extern const int abilita_vector[][16];
