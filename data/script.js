
let commandInterval;

function sendCommand(cmd) {
  fetch('/move?dir=' + cmd)
    .then(response => response.text())
    .then(text => console.log("Response: " + text));
}

function startCommand(cmd) {
  stopCommand();
  if (!cmd.startsWith("grip_")) {
    sendCommand(cmd);
    commandInterval = setInterval(() => {
      sendCommand(cmd);
    }, 100);
  } else {
    sendCommand(cmd);
  }
}

function stopCommand() {
  if (commandInterval) {
    clearInterval(commandInterval);
    commandInterval = null;
  }
  sendCommand('stop');
}

document.getElementById("m1-left").addEventListener("mousedown", () => startCommand("m1_left"));
document.getElementById("m1-left").addEventListener("touchstart", () => startCommand("m1_left"));
document.getElementById("m1-right").addEventListener("mousedown", () => startCommand("m1_right"));
document.getElementById("m1-right").addEventListener("touchstart", () => startCommand("m1_right"));

document.getElementById("m2-forward").addEventListener("mousedown", () => startCommand("m2_forward"));
document.getElementById("m2-forward").addEventListener("touchstart", () => startCommand("m2_forward"));
document.getElementById("m2-backward").addEventListener("mousedown", () => startCommand("m2_backward"));
document.getElementById("m2-backward").addEventListener("touchstart", () => startCommand("m2_backward"));

document.getElementById("m3-diag1").addEventListener("mousedown", () => startCommand("m3_diag1"));
document.getElementById("m3-diag1").addEventListener("touchstart", () => startCommand("m3_diag1"));
document.getElementById("m3-diag2").addEventListener("mousedown", () => startCommand("m3_diag2"));
document.getElementById("m3-diag2").addEventListener("touchstart", () => startCommand("m3_diag2"));

document.getElementById("grip-open").addEventListener("click", () => sendCommand("grip_open"));
document.getElementById("grip-close").addEventListener("click", () => sendCommand("grip_close"));

document.addEventListener("mouseup", stopCommand);
document.addEventListener("touchend", stopCommand);
document.addEventListener("touchcancel", stopCommand);
