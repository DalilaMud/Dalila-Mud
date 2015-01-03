module.exports = {
    
    fixRoomExits: function (source, room, new_loc, new_dir) {
          for (var i = 0; i < source.length; i++) {
            if (source[i].location === room) {
                console.log('Modifiche rooms ' + room);
                var doors = parser.editRoomExit(source[i].exits, new_loc, new_dir);
                source[i].exits = doors;
            }
          //      console.log(source[i]);
          }
         //console.log(source);
              return source;
    },
    editRoomExit: function (source, value, new_dir) {
      var exits = Array();
      var founded = false;
      for (var i = 0; i < source.length; i++) {
        if (source[i].direction === value) {
                if (new_dir == 'REM')
                {
                    source.splice(i,1);
                     console.log('Rimozione direzione ' + value);
                } else {
                    source[i].location = new_dir;
                    founded = true;
                    console.log('aggiornamento direzione ' + value);
                }
            }
     //     console.log(source[i]);
        exits.push(source[i]);
      }
      return exits;
    },
    splitZone: function (source, filter) {
        var file ='splitzone.json';
        var filtered = Array();      
       
        function readContent(callback) {
            fs.readFile(file, function (err, data) {
                if (err) return callback(err);
                var zone = JSON.parse(data);
                for (var i = 0; i < zone.length; i++) {
                    var vnum = zone[i].location;
                    for (var c = 0; c < source.length; c++) {
                        if (source[c].location == vnum) {
                            source[c].area = filter;
                            filtered.push(source[c]);
                            callback(null, filtered)
                        }
                    }

                }
                // console.log(filtered);
            });
        }

readContent(function (err, content) {
      return filtered;
})         
        
  
        
    },
    writeOnDisk: function (obj, file, type) {
    switch(type) {
            case 'json':
                 var extension = 'json';
                 var rooms = JSON.stringify(obj);
            case 'yaml':
                 var extension = 'yml';
                 var rooms = yaml.dump (obj);
            case 'csv':
                 var extension = 'csv';
                 json2csv({data: obj, fields: ['location', 'title',]}, function(err, rooms) {
                 if (err) console.log(err);
                 }); 
            default:
                 fs.writeFile(file + '.' +extension, rooms, function (err) {
                     if (err) throw err;
                     console.log('...Scrittura del file ' + type);
                 });
           }    
    
    }

}
