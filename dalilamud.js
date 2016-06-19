/********************************************************************
 * Dalila Mud Server on Node JS *************************************
 ********************************************************************/
var express = require('express');
var app = express();
var http = require('http').Server(app);

var io = require('socket.io')(http);

  //////////////////////////////////////////////////////////
 // Localization [Supporto linguistico]
//--------------------------------------------------------
var Localize = require('localize');
var _x = new Localize(__dirname + '/lang', "mm/dd/yyyy", 'it-RM'); // Lingua di default Dialetto Romano...
_x.setLocale('en');


  /////////////////////////////////////////////////////////
 // Routing Web Server
//-------------------------------------------------------
// Configure Server 
app.set('port', (process.env.PORT || 4000));
// static directory
app.use(express.static(__dirname + '/public'));
// views is directory for all template files
app.set('views', __dirname + '/views');
app.set('view engine', 'ejs');

app.get('/', function(req, res) {
  var bLang = req.acceptsLanguages( 'it', 'en' ) || 'en';
  _x.setLocale(bLang);
  console.log(_x.translate('NEW_CON_WEB', bLang));
  res.render('pages/index');
});

// Prima di lanciare il server caricare il Mud...

// Boot DB

// Start Server ///////////////////////
http.listen(app.get('port'), function() {
  console.log(_x.translate('MUD_BOOT', app.get('port')));
});


  /////////////////////////////////////
 // Socket IO ////////////////////////
/////////////////////////////////////
var numUsers = 0;

io.on('connection', function(socket){
  var addedUser = false;

  // when the client emits 'new message', this listens and executes
  socket.on('new message', function (data) {
    // we tell the client to execute 'new message'
    socket.broadcast.emit('new message', {
      username: socket.username,
      message: data
    });
  });

  // when the client emits 'add user', this listens and executes
  socket.on('add user', function (username) {
    if (addedUser) {
      console.log('Tentativo doppia connessione sullo stesso socket per ', username);
      return;
    }

    // we store the username in the socket session for this client
    socket.username = username;
    ++numUsers;
    addedUser = true;
    socket.emit('login', {
      numUsers: numUsers
    });
    // echo globally (all clients) that a person has connected
    socket.broadcast.emit('user joined', {
      username: socket.username,
      numUsers: numUsers
    });
  });

  // --- TYPING da rimuovere...
  
  // when the client emits 'typing', we broadcast it to others
  socket.on('typing', function () {
    socket.broadcast.emit('typing', {
      username: socket.username
    });
  });

  // when the client emits 'stop typing', we broadcast it to others
  socket.on('stop typing', function () {
    socket.broadcast.emit('stop typing', {
      username: socket.username
    });
  });

  // when the user disconnects.. perform this
  socket.on('disconnect', function () {
    if (addedUser) {
      --numUsers;

      // echo globally that this client has left
      socket.broadcast.emit('user left', {
        username: socket.username,
        numUsers: numUsers
      });
    }
  });
  
});


