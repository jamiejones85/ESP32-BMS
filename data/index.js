/** @brief excutes when page finished loading. Creates tables and chart */
var output, websocket,chartSpeed

function toggleHidden(id) {
	const element = document.getElementById(id);
	if (element.classList.contains('hidden')) {
		element.classList.remove('hidden');

	} else {
		element.classList.add('hidden');

	}

	return false
}
function show(id) {
	const element = document.getElementById(id);
	if (element.classList.contains('hidden')) {
		element.classList.remove('hidden');

	}

}

function hide(id) {
	const element = document.getElementById(id);
	if (!element.classList.contains('hidden')) {
		element.classList.add('hidden');

	}

}
function setValue(id, value) {
	const element = document.getElementById(id);
	element.innerHTML = value
}

function updateText(key, data) {
		if (key == 'status') {
			setValue('mode', data.replace('"', ""));
		if (data == 'Charge') {
			show('outlander-charger')
			show('chargeContainer');
		} else {
			hide('outlander-charger')
			hide('chargeContainer');

		}
	} else if (key == 'inverterstatus.val') {
		if (data == 1) {
			setValue('inverterStatus', "Run");
		} else {
			setValue('inverterStatus', "Off");

		}

		
	} else if (key == 'minVolt') {
		setValue('celllow', Math.round(data * 1000));
	} else if (key == 'maxVolt') {
		setValue('cellhigh', Math.round(data * 1000));
	} else if (key == 'cellDelta') {
		setValue('celldelta', Math.round(data * 1000));
	} else if (key == 'chargeEnabled') {
		setValue('acPresent', data);
	}  else if (key == 'requestedchargecurrent.val') {
		setValue('requestedChargeCurrent', data / 10);
	} else if (key == 'chargerstatus.val') {
		setValue('chargerStatus', data)
	} else if (key == 'chargevsetpoint.val') {
		setValue('chargeSetpoint', data)
	} else if (key == 'chargecurrentmax.val') {
		setValue('chargeMaxCurrent', data)
	} else if (key == 'evse_duty.val') {
		setValue('evseDuty', data);
	} else if (key == 'socOverride.val') {
		setValue('socOverride', data);
	} else if (key == 'amphours.val') {
		setValue('ah', data);
	} else if (key == 'capacity.val') {
		setValue('usableAh', data);
	}
}

function updateButton(key, value) {
	if (key == 'ac.val') {
		if (value == '1') {
			hide('acOverride');
			show('acOverrideOff');
		} else {
			show('acOverride')
			hide('acOverrideOff');
		}
	}
		
}

function initHandlers() {
	const acOverride = document.getElementById("acOverride");
	acOverride.onclick = function() {
		acOverride.innerHTML = "Loading"
		var xmlhttp = new XMLHttpRequest();
		xmlhttp.onreadystatechange = function() {
			acOverride.innerHTML = "Override AC Present"
		};
		xmlhttp.open("POST", "cmd", true);
		xmlhttp.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
		xmlhttp.send("cmd=a");
	}

	const shuntReset = document.getElementById("shuntReset");
	shuntReset.onclick = function() {
		var xmlhttp = new XMLHttpRequest();
		xmlhttp.open("POST", "cmd", true);
		xmlhttp.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
		xmlhttp.send("cmd=r");
	}

	const acOverrideOff = document.getElementById("acOverrideOff");
	acOverrideOff.onclick = function() {
		acOverrideOff.innerHTML = "Loading"
		var xmlhttp = new XMLHttpRequest();
		xmlhttp.onreadystatechange = function() {
			acOverrideOff.innerHTML = "Remove AC Present Override"
		};
		xmlhttp.open("POST", "cmd", true);
		xmlhttp.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
		xmlhttp.send("cmd=o");
	}

}

function dashboard() {
	var xhr = new XMLHttpRequest();

	xhr.open('GET', '/dashboard')
	xhr.onload = function() {
		const json = JSON.parse(xhr.responseText);
		onMessage(json)
	};
	xhr.send()
}

function onLoad() {
	output = document.getElementById("output");
	initGauges();
	initHandlers();
	chargerWebSocket("ws://"+ location.host +"/ws");
	setInterval(dashboard, 1000);  
}

function onOpen(evt) {
	console.log("Socket Connected");
}
   
function onMessage(json) {
	Object.keys(json).forEach(function(key) {
		updateGauge(key, json[key]);
		updateText(key, json[key]);
		// updateButton(json.message);
	});


 }

 function onError(evt) {
	console.log("Socket Error")
 }
   
 function doSend(message) {
	websocket.send(message);
 }
   
 function writeToScreen(message) {
	var pre = document.createElement("p"); 
	pre.style.wordWrap = "break-word"; 
	pre.innerHTML = message; output.appendChild(pre);
 }

 function chargerWebSocket(wsUri) {
	websocket = new WebSocket(wsUri);
	   
	websocket.onopen = function(evt) {
	   onOpen(evt)
	};

	websocket.onclose = function(evt) {
		console.log(evt)
	};
   
	websocket.onmessage = function(evt) {
		console.log(evt)
	};
   
	websocket.onerror = function(evt) {
	   onError(evt)
	};
 }

 function fileSelected()
{
}

/** @brief uploads file to web server, if bin-file uploaded, starts a firmware upgrade */
function uploadFile() 
{
	var xmlhttp = new XMLHttpRequest();
	var form = document.getElementById('uploadform');
	
	if (form.getFormData)
		var fd = form.getFormData();
	else
		var fd = new FormData(form);
	var file = document.getElementById('updatefile').files[0].name;

	xmlhttp.onload = function() 
	{
		document.getElementById("bar").innerHTML = "<p>Upload complete</p>";
		if (file.endsWith(".hex"))
		{
			runUpdate(-1, "/" + file);
		} else {
			setTimeout(function() { document.getElementById("bar").innerHTML = "" }, 5000);
		}
	}

	xmlhttp.open("POST", "/edit");
	xmlhttp.send(fd);
}