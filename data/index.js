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

function updateText(key, value) {
	if (key == 'firm.val') {
		setValue('version', value);
	} else if (key == 'status') {
		setValue('mode', value);
		// if (value == '"Charge"') {
			show('outlander-charger')
			show('chargeContainer');
		// } else {
		// 	hide('outlander-charger')
		// 	hide('chargeContainer');
		// }
	} else if (key == 'inverterstatus.val') {
		if (parts[1] == 1) {
			setValue('inverterStatus', "Run");
		} else {
			setValue('inverterStatus', "Off");

		}

		
	} else if (key == 'minVolt') {
		setValue('celllow', value);
	} else if (key == 'maxVolt') {
		setValue('cellhigh', value);
	} else if (key == 'cellDelta') {
		setValue('celldelta', value);
	} else if (key == 'ac.val') {
		setValue('acPresent', value);
	}  else if (key == 'chargerRequestedCurrent') {
		setValue('requestedChargeCurrent', value / 10);
	} else if (key == 'chargerStatus') {
		const intVal = parseInt(value)
		if (intVal == 0) {
			setValue('chargerStatus', "Not Charging")
		} else if (intVal == 0x04) {
			setValue('chargerStatus', "Wait for Mains")
		} else if (intVal == 0x08) {
			setValue('chargerStatus', "Ready/Charging")
		}
	} else if (key == 'chargevsetpoint.val') {
		setValue('chargeSetpoint', value)
	} else if (key == 'chargecurrentmax.val') {
		setValue('chargeMaxCurrent', value)
	} else if (key == 'evseDuty') {
		setValue('evseDuty', value);
	} else if (key == 'socOverride.val') {
		setValue('socOverride', value);
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

	let timerId = setInterval(requestData, 1000)
}

function onOpen(evt) {
	console.log("Socket Connected");
}
   
function onMessage(json) {
	const keys = Object.keys(json);
	keys.forEach(function(k) {
		updateGauge(k, json[k]);
		updateText(k, json[k]);
		updateButton(k, json[k]);
	})

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

function requestData() {
	var xmlhttp = new XMLHttpRequest();
	xmlhttp.onload = function() {
		var state = this.readyState;
		var responseCode = xmlhttp.status;
		if (state == this.DONE && responseCode == 200) {
			var responseData = this.responseText;
			const json = JSON.parse(responseData);
			onMessage(json)
		}
	}
	xmlhttp.open("GET", "/dashboard");
	xmlhttp.send();
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