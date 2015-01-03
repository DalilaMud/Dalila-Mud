<?php
  
function load_room($mode) {
  global $room_trovati;

  //Dir obj finder
    $dir = "../lib/world/wld";
	if ( is_dir( $dir ) && $dh = opendir( $dir ) ) {

    while ( ( $file_home = readdir( $dh ) ) !== false ) {
$stop++;
      if ( substr( $file_home, -4 ) == '.wld' ) { 

        if ($mode=="load") {

          $file = $dir."/".$file_home;
	        if (!($hfl = fopen($file, "r+b"))) {
		        echo "File ROOM non apribile (".$file_home.")\n";
            die();
	        }
          else {
            // gather statistics
            $fstat = fstat($hfl);
            if ($fstat['size'] > 0)
              $ret = read_room_file($hfl, $file_home);
            fclose($hfl);
          }

        }
        $home++;
        $order[$home] = $file_home;
      }
//if ($stop > 3) {echo "Test mode only 3 files ToT room: ".$room_trovati."<br />"; die();}
// echo "ToTaLe Room trovate in $file_home: ".$room_trovati."\n";
    }
      
  }
  else {
    echo "Dir not found.\n"; }

}

// Legge un File ROOM
function read_room_file($fl, $name) {
  global $room_trovati, $uka, $uka2;

  $uka = $name;
  $zona = str_replace(".wld", "", $name);

  $tx .= 'NOME:'.$name.'\n';

	while (!feof($fl)) {


		$importline = rtrim(fgets($fl));
    //            echo mb_detect_encoding($importLine, "auto");

    if ( false !== strpos( $importline, '#' ) ) {
      $nob++;
			sscanf($importline, "#%d", $num_obj);
      $uka2 = $num_obj;
      $res= parse_room($fl, $zona, $num_obj);
			continue;
		}
    else if ( false !== strpos( $importline, '$' ) ) {
      $room_trovati += $nob;
			$tx .= "End of File [".$nob."] nel file.\n";
			break;
		}
    else
      $tx .= $importline."\n";
  }//End While

  return $res;
}
function parse_room($fl, $zona, $nr) {
// PEr qualche ragione strana, EST e OVEST vengono fuori invertite, e io le inverto qui
$dir_name = array("nord","ovest","sud","est","alto","basso");
/* SECT_ */
$sector_types = array(	"Interno",	"Citta'",	"Campo aperto",	"Foresta",	"Collina",	"Montagna",	"Acqua (Nuoto)",	"Acqua (No Nuoto)",	"Sottacqua",
	"In Volo",	"Strada",	"Arsenale" );

  $nome=""; $short_desc=""; $long_desc=""; $act_desc=""; $clan_id=""; $master_id=""; $flag1=""; $flag2 = "";
  $alignment = 0; $tipo = 0; $lvl = 0; $hitroll = ""; $armor = "";$hp=0; $mana=0;$move=0;$damn=0;$dams=0;$damr=0;$gold=0;$exp=0;$pos=0;$def=0;$sex=0;
  $a=""; $b=""; $c=""; $d=""; $e=""; $f=""; 

  $one = parse_sound(fread_str($fl, $nr));
  $nome = colorixe($one['txt']);

  $two = parse_sound(fread_str($fl, $nr));
  $short_desc = colorixe($two['txt']);

  if ($one['sou']) {$msp = ' MSP1['.$one['sou'].']'; $mxp=$one['sou'];}
  if ($two['sou']) {$msp = ' MSP2['.$two['sou'].']'; $mxp=$two['sou'];}

  $ret = '<fieldset class="fieldo"><legend>'.$nr.'</legend>'.$nome.'<br />'.$short_desc.'<br />'.$msp.'\n';

  //Numeric Arg
  $line = rtrim(fgets($fl));
  sscanf($line, " %d %s %d ", $a, $b, $c);

  $zone = $a; //ignorata dal codice

  if (!is_numeric($b)) {$ret .= " ROOM_BIT Need AsciiConvert ";}

  $flag1 = serialize( leggiBit($b, "room_bit", 8, "binstr") );

  $tipo = $sector_types[$c];

  $ret .= 'Tipo settore '.$tipo.' Zona '.$a.'\n RoomBit '.$flag1.'\n';

  $stop = 0; $cnt_a=0; $cnt_e=0; $cnt_d=0; $exit=""; $extrad="";
  do {
    $stop++;
    $bstp = ftell($fl);
    $line = rtrim(fgets($fl));

    if ( false !== strpos( $line, 'E' ) ) {
      $cnt_e++;
      $key = colorixe(fread_str($fl, $nr));
      $e_desc = colorixe(fread_str($fl, $nr));
      $extrad[$cnt_e]['key']=$key;
      $extrad[$cnt_e]['desc']=$e_desc;
      $ret .= 'EXTRA DESC: \n Key: '.$key.'\n Desc: '.$e_desc.'\n';
    }
    else if ( false !== strpos( $line, 'F' ) ) {
      $cnt_a++; $a=0;
      $line = rtrim(fgets($fl));
      sscanf($line, " %d %d %d %d %d %d %d %d %d %d", $a, $b, $b, $b, $b, $b, $b, $b, $b, $b);
//      $ret .= 'Spec Proc (usato solo il primo gli altri sono spares): '.$line.'\n';
      if ($a == 1) {$ret .= "DUMP <br />"; $specproc="dump";}
      else if ($a == 2) {$ret .= "PET SHOP \n"; $specproc="pet_shop";}
      else if ($a == 3) {$ret .= "WAR ROOM \n"; $specproc="war_room";}
    }
    else if ( false !== strpos( $line, 'D' ) ) {
      sscanf($line, "D%d", $num_dir);
//      $ret .= '<br />'.$dir_name[$num_dir]."<br />";
      $g_desc = colorixe(fread_str($fl, $nr));
      $e_key = colorixe(fread_str($fl, $nr));

      $a="";$b="";$c="";$d="";
      $line = rtrim(fgets($fl));
      sscanf($line, " %d %d %d %d", $a, $b, $c, $d);

      $exit[$cnt_d]['direction']=$dir_name[$num_dir];
      $exit[$cnt_d]['key']=$e_key;
      $exit[$cnt_d]['desc']=$g_desc;
      $exit[$cnt_d]['info_old']=$a;
      $exit[$cnt_d]['info_new']=$d;
      $exit[$cnt_d]['chiave']=$b;
      $exit[$cnt_d]['location']=$c; // goto

//      $ret .= 'Key: '.$e_key.'\n Desc: '.$g_desc.'\n';
//      $ret .= 'Exit INFO old['.$a.'] new['.$d.'] CHIAVE n°['.$b.'] GOTO-Room['.$c.']\n';
      $cnt_d++;
    }
    /*
    else if ( false !== strpos( $line, 'S' ) ) {
      $stopb = 0; $cnt_t = 0; $done_b=""; $trigger="";
      do {
        $stopb++;
        $bstpb = ftell($fl);
        $line = rtrim(fgets($fl));

        if ( false !== strpos( $line, 'T' ) ) {
          $a="";
          sscanf($line, "T %d", $a);
          $trigger[$cnt_t]=$a;
          $cnt_t++;
          $ret .= 'TRIGGER: '.$a.'<br />';
        }
        else if ( false !== strpos( $line, '#' ) ) {
          $ret .= 'END of Room Trigger space.<br />';
          fseek($fl, $bstpb);
          $done_b = true;
        }
        else if ( false !== strpos( $line, '$' ) ) {
          fseek($fl, $bstpb);
          $done_b = true;
        }
        if ($stopb > 100) {echo "Loop in While for E mob!?"; // die();
                          };
      } while(!$done_b);

      $ret .= "End of Room <br />";
      $done = true;
    }
    */
    else if ( false !== strpos( $line, '#' ) ) {
      fseek($fl, $bstp);
      $done = true;
    }
    else if ( false !== strpos( $line, '$' ) ) {
      fseek($fl, $bstp);
      $done = true;
    }
    else {
      fseek($fl, $bstp);
      $done = true;
    }

    if ($stop > 100) {echo "Loop in While!?"; die();};
  } while(!$done);

  $ret .= '</fieldset>';

  // Scrittura dati nell'oggetto globale
    
//    $descr_it = array ('it'           => mb_convert_encoding($short_desc, "UTF-8"));
    $descr_it = array ('it'           => removeNL($short_desc));
    $title_it = array ('it'           => $nome);
    $objRoom = array ( 'location'       => $nr, //VNUM
                      'area'          => $zona, //Zona
                      'title'         => $title_it, //name
                      'description'   => $descr_it,
                      'tipo'          => $tipo,
                      'room_bit'      => $flag1,
                      'extra_desc'    => $extrad,
                      'procedure'     => $specproc,
                      'exits'        => $exit,
                      'trigger'       => $trigger,
                      'music'         => "null"

                    );

   array_push ($GLOBALS['objRooms'], $objRoom);     

//  print_r($objRoom);
  return $ret;

}
?>
