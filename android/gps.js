var t = new Date();
moving = false;
last_angle = 0;
new_angle = 0;
next_action = 0;

var usb;

//Called when application is started.
function OnStart()
{
    app.SetOrientation( "Portrait" );
	//Create vertical layout that fills the screen.
	lay = app.CreateLayout( "Linear", "FillXY,VCenter" );
 
	//Create a text control to show data log.
	txt = app.CreateText( "Locating...", 0.9, 0.1, "Multiline,Left" );
	txt.SetTextColor( "#ffffffff" );
	lay.AddChild( txt ); 
	
	//Create a text control to show data log.
	txt3 = app.CreateText( "Orientation...", 0.9, 0.1, "Multiline,Left" );
	txt3.SetTextColor( "#ffffffff" );
	lay.AddChild( txt3 );
	
    //Create a text control to show data log.
	capture = app.CreateText( "Tap 'Set destination' to see destination here.", 0.9, 0.2, "Multiline,Left" );
	capture.SetTextColor( "#ffffffff" );
	lay.AddChild( capture );

	layBut = app.CreateLayout("Linear", "Horizontal"); 
    lay.AddChild( layBut ); 
	
	//Create a button 1/3 of screen width and 1/10 screen height.
	btnGo = app.CreateButton( "Go!", 0.2, 0.1 );
	btnGo.SetMargins( 0, 0.05, 0, 0 );
	btnGo.SetEnabled(false);
	layBut.AddChild( btnGo );
	btnGo.SetOnTouch( btnGo_OnTouch );
	
	/*btn2 = app.CreateButton( "START!", 0.2, 0.1 );
	btn2.SetMargins( 0, 0.05, 0, 0 );
	layBut.AddChild( btn2 );
	
	//Set function to call when button pressed.
	btn2.SetOnTouch( btn2_OnTouch );*/
	
    btnSetDestination = app.CreateButton( "Set destination", 0.3, 0.1 );
	btnSetDestination.SetMargins( 0, 0.05, 0, 0 );
	btnSetDestination.SetEnabled(false);
	layBut.AddChild( btnSetDestination );
	btnSetDestination.SetOnTouch( btnSetDestination_OnTouch );

	btnCopy = app.CreateButton( "Copy", 0.2, 0.1 );
	btnCopy.SetMargins( 0, 0.05, 0, 0 );
	layBut.AddChild( btnCopy );
	btnCopy.SetOnTouch( btnCopy_OnTouch );
	
	btnCopy2 = app.CreateButton( "Copy2", 0.2, 0.1 );
	btnCopy2.SetMargins( 0, 0.05, 0, 0 );
	layBut.AddChild( btnCopy2 );
	btnCopy2.SetOnTouch( btnCopy2_OnTouch );
	
	//Add layout to app.	
	app.AddLayout( lay );
	
	usb = app.CreateUSBSerial(74880);
    
	gps = app.CreateLocator( "GPS" );
	gps.SetRate(1);
	gps.SetOnChange(gps_newPosition);
	gps.Start();
	
	orient = app.CreateSensor( "Orientation", "slow" );
	orient.SetOnChange( orient_OnChange );  
	orient.SetMinChange( 0.5 ); 
	orient.Start();
	
	log = new Array(); 
}

function orient_OnChange( azimuth, pitch, roll, time )
{
	txt3.SetText( "Azimuth = " + azimuth.toFixed(1) + "\n Pitch=" 
		+ pitch.toFixed(1) + "\n Roll = " + roll.toFixed(1) );
		
		last_angle = azimuth;
		
}

function get_angle(p1, p2)
{
  var dx = p1[0] - p2[0];
  var dy = p1[1] - p2[1];
  var rads = Math.atan2(dy, dx);
  if (rads < 0) {
    rads = Math.abs(rads);
  } else {
    rads = 2 * Math.PI - rads;
  }
  return 270 - (rads * (180/Math.PI));
}
function shortest_rotation(a1, a2)
{
  return (a2 - a1 + 540) % 360 - 180;
}
function distance_between(p1, p2)
{
  var a = p1[0] - p2[0];
  var b = p1[1] - p2[1];
  return Math.sqrt( a*a + b*b );
}

// Do the navigation. angle_current 0-360, 0 is north, 90 is east
function navigate(coords_current, coords_destination, angle_current)
{
    // Input
    var angle = get_angle(coords_current, coords_destination);
    var rotation = shortest_rotation(angle_current, angle);
    var distance = distance_between(coords_current, coords_destination);

    // Output
    var x = rotation / 180 * 100; // Vertical movement -100 (full left) to +100 (full right)
    if (Math.abs(rotation) > 45) {
      var y = 0; // Horizontal movement -100 (backwards) to +100 (forwards)
    } else {
      var y = Math.max(distance, 100);
    }
    return [x,y];
}

var current_pos;
function gps_newPosition(data)
{
    btnSetDestination.SetEnabled(true);
    current_pos = data;
    var t = "New position: " + data.provider+": Lat "+data.latitude+", Lng "+data.longitude 
		+", Spd "+data.speed+", Bear "+data.bearing
		+", Alt "+data.altitude+", Accu "+data.accuracy;

		
	if(destination!=null && destination!=current_pos)
	    t += " Destination: Lat "+destination.latitude+", Lng "+destination.longitude;
	
	txt.SetText(t);
}

direction1 = 0, direction2 = 0;
function keep_moving()
{
	if(moving)
	{
	    var move = navigate([current_pos.latitude,current_pos.longitude],[destination.latitude,destination.latitude],last_angle);

        if(log.length>4)
            log.shift();
            
        log.push("moving! " + move[0] + ", " + move[1]);
	    capture.SetText( log.join("\n") );

        if(move[0] == 0 && move[1] == 0) 
	        moving = false;
	    else
	        setTimeout (keep_moving, 100);
	}	
}

//Called when user touches our button.
function btnGo_OnTouch()
{
	moving=true;
	setTimeout (keep_moving, 0);
}


//Called when user touches our button.
function btn2_OnTouch()
{
	moving=true;
	setTimeout (keep_moving, 0);
}

var destination;
//Called when user touches our button.
function btnSetDestination_OnTouch()
{
    btnGo.SetEnabled(true);
	destination = current_pos;	
}

//Called when user touches our button.
function btnCopy2_OnTouch()
{
	app.SetClipboardText( capture.GetText() );
	
	if(usb==null)
	    app.ShowPopup('Usb not connected');
	else
	    usb.Write("stop");
}

//Called when user touches our button.
function btnCopy_OnTouch()
{
	app.SetClipboardText( capture.GetText() );
	
	if(usb==null)
	    app.ShowPopup('Usb not connected');
	else
	    usb.Write("move 50 50");
	
}