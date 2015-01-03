yaml = require('js-yaml');
json2csv = require('json2csv');
fs   = require('fs');

function fixExits(source, room, new_loc, new_dir) {
  //  var new_rooms = Array ();
  for (var i = 0; i < source.length; i++) {
    if (source[i].location === room) {
        var doors = editExit(source[i].exits, new_loc, new_dir);
        source[i].exits = doors;
    //    console.log(source[i]);
    }
  //      console.log(source[i]);
  }
    // console.log(source);
          return source;
}
function editExit(source, value, new_dir) {
  var exits = Array();
  var founded = false;
  for (var i = 0; i < source.length; i++) {
    if (source[i].direction === value) {
        source[i].location = new_dir;
        founded = true;
    }
  //    console.log(source[i]);
    exits.push(source[i]);
  }
  return exits;
}


fs.readFile('rooms.json', 'utf8', function (err, data) {
  if (err) {
    console.log('Error: ' + err);
    return;
  }
  var rooms = JSON.parse(data);

    // Modifiche da applicare ai file wld originali
    fixExits(rooms, 21201, 'nord', '31');
    fixExits(rooms, 21301, 'sud', '13');
    fixExits(rooms, 21385, 'ovest', '30');
    fixExits(rooms, 21315, 'ovest', '30');
    
  var numRooms = rooms.length;
    
 //   FOR JSON Output    
 //   console.log(rooms);

 // FOR YAML output
  var roomsYaml = yaml.dump (rooms);
  console.log(roomsYaml);

  // FOR CSV Output  
// json2csv({data: rooms, fields: ['location', 'title',]}, function(err, csv) {
//  if (err) console.log(err);
//  console.log(csv);
// });    
    

});
