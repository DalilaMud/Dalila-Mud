yaml = require('js-yaml');
json2csv = require('json2csv');
fs   = require('fs');
parser = require('./parse');



fs.readFile('rooms.json', 'utf8', function (err, data) {
  if (err) {
    console.log('Error: ' + err);
  //  return;
  }
  var rooms = JSON.parse(data);

    // Modifiche da applicare ai file wld originali

    parser.fixRoomExits(rooms, 21201, 'nord', '31');
    parser.fixRoomExits(rooms, 21201, 'nord', '31'); // Porta nord
    parser.fixRoomExits(rooms, 21301, 'sud', '13');    //porta sud
    parser.fixRoomExits(rooms, 21385, 'ovest', '30');  // porta est
    parser.fixRoomExits(rooms, 21285, 'ovest', 'REM');  // rimozione siamese dream
    parser.fixRoomExits(rooms, 21314, 'sud', 'REM');  // rimozione kame house
    parser.fixRoomExits(rooms, 21284, 'alto', 'REM');  // edificio in costruzione
    parser.fixRoomExits(rooms, 40549, 'alto', 'REM');  // edificio in costruzione
    parser.fixRoomExits(rooms, 21287, 'sud', 'REM');  // accorcia la via degli dei a sud
 
//    parser.splitZone(rooms, 'chyrellos');
//    console.log(p);
    
 // var numRooms = rooms.length;
 //  console.log(numRooms);
    
    parser.writeOnDisk(rooms, 'roomsFiltered', 'yaml');
                

    

});
