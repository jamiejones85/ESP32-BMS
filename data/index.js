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

function updateText(data) {
	const parts = data.split("=");
	if (parts[0] == 'firm.val') {
		setValue('version', parts[1]);
	} else if (parts[0] == 'stat.txt') {
		setValue('mode', parts[1]);
		if (parts[1] == '"Charge"') {
			show('outlander-charger')
			show('chargeContainer');
		} else {
			hide('outlander-charger')
			hide('chargeContainer');

		}
	} else if (parts[0] == 'inverterstatus.val') {
		if (parts[1] == 1) {
			setValue('inverterStatus', "Run");
		} else {
			setValue('inverterStatus', "Off");

		}

		
	} else if (parts[0] == 'lowcell.val') {
		setValue('celllow', parts[1]);
	} else if (parts[0] == 'highcell.val') {
		setValue('cellhigh', parts[1]);
	} else if (parts[0] == 'celldelta.val') {
		setValue('celldelta', parts[1]);
	} else if (parts[0] == 'ac.val') {
		setValue('acPresent', parts[1]);
	}  else if (parts[0] == 'requestedchargecurrent.val') {
		setValue('requestedChargeCurrent', parts[1] / 10);
	} else if (parts[0] == 'chargerstatus.val') {
		setValue('chargerStatus', parts[1])
	} else if (parts[0] == 'chargevsetpoint.val') {
		setValue('chargeSetpoint', parts[1])
	} else if (parts[0] == 'chargecurrentmax.val') {
		setValue('chargeMaxCurrent', parts[1])
	} else if (parts[0] == 'evse_duty.val') {
		setValue('evseDuty', parts[1]);
	} else if (parts[0] == 'socOverride.val') {
		setValue('socOverride', parts[1]);
	}
}

function updateButton(data) {
	const parts = data.split("=");
	if (parts[0] == 'ac.val') {
		if (parts[1] == '1') {
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

	const saveSettings = document.getElementById("saveSettings");
	saveSettings.onclick = function() {
		var xmlhttp = new XMLHttpRequest();
		xmlhttp.open("POST", "cmd", true);
		xmlhttp.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
		xmlhttp.send("cmd=s");
	}

	bindSettingsButton("btnUpdateChargeSetpoint", "updateChargeSetpoint", "v")
	bindSettingsButton("btnUpdateChargeMaxCurrent", "updateChargeMaxCurrent", "c")
	bindSettingsButton("btnSocOverride", "updateSocOverride", "q")


}

function bindSettingsButton(buttonId, inputId, command) {
	const button = document.getElementById(buttonId);
	button.onclick = function() {
		const updateInput = document.getElementById(inputId);
		var xmlhttp = new XMLHttpRequest();
		xmlhttp.open("POST", "cmd", true);
		xmlhttp.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
		xmlhttp.send("cmd=" + command + " "  + updateInput.value);
	}
}

function onLoad() {
	output = document.getElementById("output");
	initGauges();
	initHandlers();
	chargerWebSocket("ws://"+ location.host +":81");
}

function onOpen(evt) {
	console.log("Socket Connected");
}
   
function onMessage(evt) {
	const json = JSON.parse(evt.data);
	updateGauge(json.message);
	updateText(json.message);
	updateButton(json.message);
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
	   onMessage(evt)
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