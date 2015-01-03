#!/usr/bin/php
<?php
// TODO
// Convert *.wld in utf-8 format  iconv -f ISO-8859-1 -t UTF-8
// Remove $~ for the end of *.wld
//
$GLOBALS['objRooms'] = array ();
// require_once "tool/spyc.php";  required for YAML

$objObjs = array();

class objObj {
    public $vnum = " ";
    public $short_description  = array();
    public $keywords  = array();
    public $room = " ";
    public $script = " ";
    public $behaviors = " ";
    public $attributes = array ();
    public $load_max = " ";
    // attributes: speed , damage
    public $zona = " ";
//    public $act_desc = " ";
    public $clan_id = " ";
    public $master_id = " ";
    public $action_bit = " ";
    public $affect_bit = " ";
    public $allineamento = " ";
    public $tipo = " ";
    public $hitroll = " ";
    public $mana = " ";
    public $move = " ";
    public $damnodice = " ";
    public $damsizedice = " ";
    public $damroll = " ";
    public $pos = " ";
    public $def = " ";
    public $sex = " ";
    public $trigger = " ";
    public $special = " ";
}

/*
for ($i=0;$i<2;$i++) {
$objRoom = new objRoom();
$objRoom->location = "21200";
$objRoom->description  = "Blah blah blah";
// $objRoom->exits = new objExits();

array_push($objRooms, $objRoom);
}
$j =json_encode($objRooms);
*/

include ("commons.php");
 include ("rooms.php");
//include ("mobs.php");

 load_room("load");
// include ("cleaner.php");


//load_mob("load");

// MOBS
// echo  "\n ".count($objObjs)."\n";
//  $yObj= Spyc::YAMLDump($objObjs);
//    $jObjs = json_encode_objs($objObjs);
// print_r($jObjs);

// ROOMS
// $num=count($GLOBALS['objRooms']);
// echo "ROOMS Imported : $num \n";
array_walk_recursive($GLOBALS['objRooms'], function(&$value, $key) {
    if (is_string($value)) {
        $value = utf8_encode($value);
    }
});

 // $yRooms = Spyc::YAMLDump($GLOBALS['objRooms']);  //ERROR charset :(
 $jRooms = json_encode($GLOBALS['objRooms']);
 //$jRooms = get_object_vars($GLOBALS['objRooms']);
 //   $char = preg_replace('/[\x00-\x08\x0B\x0C\x0E-\x1F\x80-\x9F]/u', '', $yaml);
  print_r ($jRooms);

function writeUTF8File($filename,$content) { 
        $f=fopen($filename,"w"); 
        # Now UTF-8 - Add byte order mark 
      //  fwrite($f, pack("CCC",0xef,0xbb,0xbf)); 
        fwrite($f,$content); 
        fclose($f); 
}
 writeUTF8File('rooms.json', $jRooms);

?>