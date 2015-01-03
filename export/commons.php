<?php
// JSONificare un oggetto
    function json_encode_objs($item){   
        if(!is_array($item) && !is_object($item)){   
            return json_encode($item);   
        }else{   
            $pieces = array();   
            foreach($item as $k=>$v){   
                $pieces[] = "\"$k\":".json_encode_objs($v);   
            }   
            return '{'.implode(',',$pieces).'}';   
        }   
    }
// export multiformato
function exportObj($method = 'a')
{
     if($method == 'j')
     {
         return json_encode(get_object_vars($this));
     }
     else
     {
         return get_object_vars($this);
     }
}

// Converte i colori usati in Dalila Mud &0->&9 ect.
function colorixe($st) {
  $res = str_replace("&0","<white>",$st);
  $res = str_replace("&d","<white>",$res);

  $res = str_replace("&1","<red>",$res);
  $res = str_replace("&r","<red>",$res);

  $res = str_replace("&2","<green>",$res);
  $res = str_replace("&v","<green>",$res);

  $res = str_replace("&3","<yellow>",$res);
  $res = str_replace("&g","<yellow>",$res);

  $res = str_replace("&4","<blue>",$res);
  $res = str_replace("&b","<blue>",$res);

  $res = str_replace("&5","<magenta>",$res);
  $res = str_replace("&e","<magenta>",$res);

  $res = str_replace("&6","<cyan>",$res);
  $res = str_replace("&c","<cyan>",$res);

  $res = str_replace("&7","<cyan>",$res);
  $res = str_replace("&w","<cyan>",$res);

  $res = str_replace("&8","<black>",$res);
  $res = str_replace("&z","<black>",$res);

  $res = str_replace("&9","<bold>",$res);
  $res = str_replace("&I","<bold>",$res);

  $res = str_replace("&u","<underline>",$res);
  $res = str_replace("&i","<reverse>",$res);
  $res = str_replace("&l","<blink>",$res);
    
// Color rosso-l
  $res = str_replace("&R","<red>",$res);
  $res = str_replace("&V","<green>",$res);
  $res = str_replace("&G","<yellow>",$res);
  $res = str_replace("&B","<blue>",$res);
  $res = str_replace("&E","<magenta>",$res);
  $res = str_replace("&C","<cyan>",$res);
  $res = str_replace("&W","<white>",$res);
  $res = str_replace("&Z","<black>",$res);
// Color rosso-s 
  $res = str_replace("&P","<red>",$res);
  $res = str_replace("&H","<green>",$res);
  $res = str_replace("&A","<yellow>",$res);
  $res = str_replace("&N","<blue>",$res);
  $res = str_replace("&T","<magenta>",$res);
  $res = str_replace("&O","<cyan>",$res);
  $res = str_replace("&M","<white>",$res);
  $res = str_replace("&X","<black>",$res);
// Color rosso-sl
  $res = str_replace("&p","<red>",$res);
  $res = str_replace("&h","<green>",$res);
  $res = str_replace("&a","<yellow>",$res);
  $res = str_replace("&n","<blue>",$res);
  $res = str_replace("&t","<magenta>",$res);
  $res = str_replace("&o","<cyan>",$res);
  $res = str_replace("&m","<white>",$res);
  $res = str_replace("&x","<black>",$res);

  // Non usati
  $res = str_replace("&f","<white>",$res);
  $res = str_replace("&q","<white>",$res);
  $res = str_replace("&s","<white>",$res);
  $res = str_replace("&y","<white>",$res);
  $res = str_replace("&k","<white>",$res);
  $res = str_replace("&j","<white>",$res);
  $res = str_replace("&D","<white>",$res);
  $res = str_replace("&F","<white>",$res);
  $res = str_replace("&L","<white>",$res);
  $res = str_replace("&Q","<white>",$res);
  $res = str_replace("&S","<white>",$res);
  $res = str_replace("&U","<white>",$res);
  $res = str_replace("&Y","<white>",$res);
  $res = str_replace("&J","<white>",$res);
  $res = str_replace("&£","<white>",$res);
  $res = str_replace("&§","<white>",$res);
  /*
  $sp = substr_count($res, '<span');
  $ps = substr_count($res, '</span');
  $tt = $sp - $ps;
  if ($tt > 0) {
    for($x=1;$x<=$tt;$x++) {$res .= "</span>";}
  }
  */
//  print_r (" $res \n");
  return $res;
}
/**
 * Remove new line (\n) and double space
 *
 */
function removeNL($str) {

$string = trim(preg_replace('/\s\s+/', ' ', $str));
    return $string;

}
/**
 * Legge una stringa nei file del mud fino al Token ~
 *
 * @param File 2 read $fp
 * @param unknown $nom
 * @return string
 */
function fread_str($fp, $nom) {
        global $uka, $uka2;

        $done = false;
        $stop = 0; $res = '';

        do {
                $tmp = rtrim( fgets( $fp ) );

                if ( false !== strpos( $tmp, '~' ) ) {
                        //php 5.3
                        //$res .= strstr($tmp, "~", TRUE);
                        //php 4.0
                        $res .= rtrim($tmp, "~");
                        $done = true;
                }
                else {
                        $stop++;
                        $res .= $tmp;
                }
                if ($stop > 200) { $res .=  "More Then 200 Lines in ".$uka." ".$uka2;
break;}

        } while(!$done);

        return $res;
}

// Rimuove il protocollo suoni MSP
function parse_sound($txt) {
  $stop = 0;
  $ret['txt'] = $txt;

	do {
    $stop++;
		$mspSou = strpos($ret['txt'],"!!SOUND(");
		$mspMus = strpos($ret['txt'],"!!MUSIC(");

		if (($mspSou !== false) || ($mspMus !== false)) {
		  if ($mspSou !== false) {
        $locIn = $mspSou;
        $mspSou = strstr($ret['txt'],"!!SOUND(");
        $locEnd = strpos($mspSou,")");
			  if ($locEnd === false) $locEnd = $locIn + 7;
        $ret['sou'] = substr($ret['txt'], ($locIn+8), ($locEnd-8));
        $ret['txt'] = substr_replace($ret['txt'], '', $locIn, ($locEnd+1));
        //$ret['dbg'] = "[".$locIn."-".$locEnd."]";
      }

		  if ($mspMus !== false) {
        $locIn = $mspMus;
        $mspMus = strstr($ret['txt'],"!!MUSIC(");
        $locEnd = strpos($mspMus,")");
			  if ($locEnd === false) $locEnd = $locIn + 7;
        $ret['sou'] = substr($ret['txt'], ($locIn+8), ($locEnd-8));
        $ret['txt'] = substr_replace($ret['txt'], '', $locIn, ($locEnd+1));
        //$ret['dbg'] = "[".$locIn."-".$locEnd."]";
      }
    }
    else
      { $locIn = "done";}

    if ($stop > 100) {$ret['sou'] = "OverWhile MSP exit"; break;}
	}	while ($locIn != "done");

  return $ret;
}
function leggiBit($sbit, $nomi, $len, $mode) {

$shop_bit = array (	"WILL_FIGHT",	"USES_BANK", "NO_SELL", "NO_BUY", "PG_PROD");

/* CONT_x8 */
$container_bit = array (
	"CLOSEABLE","PICKPROOF","CLOSED","LOCKED"
);

// ROOM_bit x64
$room_bit = array (
	"DARK",	"DEATH",	"!MOB",	"INDOORS",
	"PEACEFUL",	"SOUNDPROOF",	"!TRACK",	"!MAGIC",
	"TUNNEL",	"PRIVATE",	"GODROOM",	"HOUSE",
	"HCRSH",	"HCNTRL",	"OLC",	"BFS_MARK(Res)",
	"DAMAGE",	"ARENA",	"LANDMARK",	"NO_DRAW",
	"WAR",	"FORGIA",	"FABBRO",	"FALEGNAMERIA",
	"CARPENTERIA",
	"PIETRE_PREZIOSE",
	"GIOIELLERIA",
	"SQUADRATURA",
	"PRIGIONE",
	"ERBORISTERIA",
	"LAB_ALCHIMIA",
	"MULINO/RIPULIT",
	"CUCINA",
	"SCUOIATURA",
	"CONCERIA",
	"COSTRUZIONE",
	"STOP COSTRUZ.",
	"INGRESSO",
	"NO_BUY",
	"NO_SELL",
	"ZECCA",
	"TRADE",
	"MAGAZZINO",
	"EDIFICABILE",
	"TEMPIO",
	"!WRAITH",
	"DAMAGE_FUOCO",
	"DAMAGE_ACQUA",
	"DAMAGE_ARIA",
	"DAMAGE_TERRA",
	"BIG_HOUSE"
);

// MOB_flag x64
$action_bit = array (
	"SPEC",	     "SENTINEL",	"SCAVENGER",    "(R)ISNPC",
	"!BACKSTAB", "AGGR",      "STAY-ZONE",    "WIMPY",
  "AGGR_EVIL", "AGGR_GOOD", "AGGR_NEUTRAL",	"MEMORY",
	"HELPER",	"!CHARM",	"!SUMMN",	"!SLEEP",
	"!BASH",	"!BLIND",	"WILDHUNT",	"QUEST",
	"QUESTMASTER",
	"ASSASSIN",
	"HUNTER",
	"CAVALCATURA",
	"DRAGO",
	"SOFFIODRAGO",
	"SALVA_INV",
	"NEGOZIANTE",
	"BOUNTY KILLER",
	"NOKILL",
	"CIVILIAN",
	"CRIMINAL",
	"CERCATORE",
	"SALVA_MOB",
	"FAST-GO",
	"SELVAGGINA",
	"RIS.CARNE",
	"RIS.PELLE",
	"RIS.STOFFA",
	"RIS.PELLICCIA",
	"RIS.SCAGLIE",
	"BODYGUARD",
	"NECRO",
	"!AGGR_PROPR",
	"!AGGR CLAN ALLEAT",
	"!AGGR CLAN NEMICO",
	"!AGGR CLAN PACE",
	"!AGGR CLAN",
	"!AGGR CLAN VASS",
	"AMMAESTRABILE",
	"LAVORATORE",
	"CRIM-HELPER",  
	"MOB_CRASH",  
	"MOB_DELETE",  
	"MOB_CRASH_EQ",  
	"!XP",
	"STIPENDIATO",
	"TASSA_EQUIP",
	"ESERCITO",
	"!DECAPITA",
	"(R)NDY", "M_62", "M_63", "M_64"
);
  // PRF_x 
  $pref_bit = array(
	"BRIEF",			// 00
	"COMPACT",
	"DEAF",
	"!TELL",
	"D_HP",
	"D_MANA",
	"D_MOVE",
	"AUTOEX",
	"!HASS",
	"QUEST",
	"SUMN",				// 10
	"!REP",
	"LIGHT",
	"C1",
	"C2",
	"!WIZ",
	"L1",
	"L2",
	"!AUC",
	"!GOS",
	"!GTZ",				// 20
	"RMFLG",
	"AFK",
	"D_GOLD",
	"D_XP",
	"D_DAM",
	"DIR",
	"ARENA",
	"SOUND",
	"DET_HD_SK",
	"NO_EROE",		// 30
	"D_CAV",
	"F32",
	"NoKill",
	"!REGNO",
	"!OOC",
  "DSTTCo",
  "DSTTCa",
  "DSTTCi",
  "CLRS",
  "BWALK",			// 40
  "F41",
	"F42",
	"F43",
	"F44",
	"F45",
  "F46",
  "F47",
  "F48",
  "F49",
  "F50",				// 50
  "F51",
	"F52",
	"F53",
	"F54",
	"F55",
  "F56",
  "F57",
  "F58",
  "F59",
  "F60",				// 60
  "F61",				// 61
  "F62",				// 62
  "F63",				// 63 ...bit 64
	"\n"
);
// ITEM_x x64 versione breve x Output
$extra_bit = array(
	"GLOW",
	"HUM",
	"NORENT",
	"NODONATE",
	"NOINVIS",
	"INVISIBLE",
	"MAGIC",
	"NODROP",
	"BLESS",
	"NOGOOD",
	"NOEVIL",
	"NONEUTRAL",
	"NOPANDION",
	"NOCYRINIC",
	"NOALCIONE",
	"NOGENIDIAN",
	"NOSELL",
	"NOPELOI",
	"LIVE_GRENADE",
	"NOLOCATE",
	"NO_5_LIV",
	"NO_10_LIV",
	"NO_20_LIV",
	"NO_25_LIV",
	"NO_30_LIV",
	"NO_40_LIV",
	"RESTRING (R)",
	"CORPSE (R)",
	"FORGIATO",
	"AFFILATO",
	"RINOM_ALIAS",
	"RINOM_NAME",
	"RINOM_DESCR",
	"NO_IDENT",
	"!MAGIC"
);

// ITEM_WEAR_x32 (wear bitvector) Versione breve x gli OUTPUT
$wear_bit = array (
	"TAKE",
	"FINGER",
	"NECK",
	"BODY",
	"HEAD",
	"LEGS",
	"FEET",
	"HANDS",
	"ARMS",
	"SHIELD",
	"ABOUT",
	"WAIST",
	"WRIST",
	"WIELD",
	"HOLD",
	"LOBSX",
	"SPALLE",
	"POLSI",
	"OCCHI",
	"BOCCA",
	"ALTRO",
	"HANG",
	"VESTE",
	"RELIQUIA"
);
//AFFECTED_0 x64
$affe0_bit = array (
	"BLIND",
	"INVIS",
	"DET-ALIGN",
	"DET-INVIS",
	"DET-MAGIC",
	"SENSE-LIFE",
	"WATWALK",
	"SANCT",
	"GROUP",
	"CURSE",
	"INFRA",
	"POISON",
	"PROT-EVIL",
	"PROT-GOOD",
	"SLEEP",
	"NOTRACK",
	"MARK",
	"PASSDOOR",
	"SNEAK",
	"HIDE",
	"FIRESHD",
	"CHARM",
	"FLYING",
	"WATERBREATH",
	"PROTFIRE",
	"SHIELD",
	"DEATHDANCE",
	"MIRRORIMAGE",
	"BLINK",
	"WIZEYE",
	"PARALIZED",
	"DOMATO",
	"ACCAMPATO (R)",
	"TRAMORTITO",
	"IMMOBILIZZATO",
	"CAPTURING",
	"TRASPORTATO",
	"PROTLIGHTN",
	"DET-SNEAK",
	"MUTO",
	"DISGUISE",
	"ANTIMAGIA",
	"AID",
	"CONGIUNZIONE",
	"CRIT_TESTA_1",
	"CRIT_BRACCIO_D_1",
	"CRIT_BRACCIO_S_1",
	"CRIT_TORSO_1",
	"CRIT_GAMBA_D_1",
	"CRIT_GAMBA_S_1",
	"CRIT_TESTA_2",
	"CRIT_BRACCIO_D_2",
	"CRIT_BRACCIO_S_2",
	"CRIT_TORSO_2",
	"CRIT_GAMBA_D_2",
	"CRIT_GAMBA_S_2",
	"CRIT_TESTA_3",
	"CRIT_BRACCIO_D_3",
	"CRIT_BRACCIO_S_3",
	"CRIT_TORSO_3",
	"CRIT_GAMBA_D_3",
	"CRIT_GAMBA_S_3",
	"FERITO",
	"ENDURANCE",
	"\n"
);
// AFF_1 x64
$affe1_bit = array (
	"BURNED",	"CHILLED", "LEVITATE", "RAFFREDDATO",	"APPESTATO", "ABBACCATO",	"MALEDETTO_DA_SHAARR", "PIAGATO_ALLE_MANI",
	"!MAGIC",	"AMMORFEATO",	"B_11",	"B_12",	"B_13",	"B_14",	"B_15",	"B_16",
	"B_17",	"B_18",	"B_19",	"B_20",	"B_21",	"B_22",	"B_23",	"B_24",	"B_25",	"B_26",	"B_27",	"B_28",	"B_29",	"B_30",	"B_31",	"B_32",
	"B_33",	"B_34",	"B_35",	"B_36",	"B_37",	"B_38",	"B_39",	"B_40",	"B_41",	"B_42",	"B_43",	"B_44",	"B_45",	"B_46",	"B_47",	"B_48",
	"B_49",	"B_50",	"B_51",	"B_52",	"B_53",	"B_54",	"B_55",	"B_56",	"B_57",	"B_58",	"B_59",	"B_60",	"B_61",	"B_62",	"B_63",	"B_64",
	"\n"
);

  $rn = ($len*8);
  $sbot="";

  // Leggiamo una Stringa in Binario 01010
  if ($mode=="binstr") {
    $sbot[0] = $sbit;
    $sbit = base_convert($sbit, 10, 2);
    $sbit = strrev($sbit);
    $xero = strlen($sbit);
    $x = $$nomi;
    for ($nr=0; $sbit && $nr < $rn;) {
     if ($nr >= $xero) $sbit[$nr]="0";
      //echo "[".($nr+1)."] ".$sbit[$nr]." ".$x[$nr]."<br />";
      if ($sbit[$nr]=="1") {
        $k++;
        $sbot[$k]=$x[$nr];
      }
      $nr++;
    }
    return $sbot;
  }
  // Normal Mode passiamo un int (x32 funziona bene) (x64 solo su Sistemi a 64bit)
  else {
    $sbot[0] = $sbit;
    $x = $$nomi;
    for ($nr = 0; $sbit && $nr < $rn; $sbit >>= 1) {
      if($sbit & 1) {
        $k++;
        $sbot[$k] = $x[$nr];
      }
      $nr++;
    }
    return $sbot;
  }
}

?>