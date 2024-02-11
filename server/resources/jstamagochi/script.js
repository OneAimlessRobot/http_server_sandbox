const sleepBtn = document.querySelector("#action-sleep");
const feedBtn = document.querySelector("#action-feed");
const playBtn = document.querySelector("#action-play");
const startBtn = document.querySelector("#action-menu-start-game");
const settingsBtn = document.querySelector("#action-menu-settings");
const settingsBackBtn = document.querySelector("#action-settings-back");
const difHardBtn = document.querySelector("#action-settings-difficulty-hard");
const difNormalBtn = document.querySelector(
  "#action-settings-difficulty-normal"
);
const difEasyBtn = document.querySelector("#action-settings-difficulty-easy");
const nightModeOffBtn = document.querySelector("#nightmode-off");
const nightModeOnBtn = document.querySelector("#nightmode-on");

const sleepHp = document.querySelector("#sleep-hp");
const hungerHp = document.querySelector("#hunger-hp");
const playHp = document.querySelector("#play-hp");
const energyHp = document.querySelector("#energy-hp");
const skillLevelHp = document.querySelector("#skillLevel-hp");
const scoreBar = document.querySelector("#score");
const maxSleep = 300;
const maxHunger = 300;
const maxPlay = 300;
const maxSkillLevel = 300;
const maxEnergy = 300;

let day = 20;

function Tamagotchi() {
  this.sleep = maxSleep;
  this.hunger = maxHunger;
  this.play = maxPlay;
  this.energy = maxEnergy;
  this.skillLevel = maxSkillLevel;
}

Tamagotchi.prototype.actionSleep = function () {
  this.sleep += 40 / (day * 2);
  this.energy += 40 / (day * 2);
};

Tamagotchi.prototype.actionEat = function () {
  this.hunger += 120 / (day * 2);
};

Tamagotchi.prototype.actionPlay = function () {
  this.play += 80 / (day * 2);
  this.skillLevel += 40 / (day * 2);
};

Tamagotchi.prototype.tick = function () {
  this.sleep--;
  this.hunger -= 3;
  this.play -= 2;
};

let tmgch = new Tamagotchi();
let sleepHpCount;
let hungerHpCount;
let playHpCount;
let energyHpCount;
let skillLevelHpCount;
let score = 0;

sleepBtn.addEventListener("click", function () {
  tmgch.actionSleep();
});

feedBtn.addEventListener("click", function () {
  tmgch.actionEat();
});

playBtn.addEventListener("click", function () {
  tmgch.actionPlay();
});

startBtn.addEventListener("click", function () {
  startGame();
});

settingsBtn.addEventListener("click", function () {
  settingsMenu();
});

difHardBtn.addEventListener("click", function () {
  day = 5;
  document.querySelector("#difSet").innerHTML = "Hard";
});

difNormalBtn.addEventListener("click", function () {
  day = 20;
  document.querySelector("#difSet").innerHTML = "Normal";
});

difEasyBtn.addEventListener("click", function () {
  day = 40;
  document.querySelector("#difSet").innerHTML = "Easy";
});

settingsBackBtn.addEventListener("click", function () {
  MainMenu();
});

nightModeOffBtn.addEventListener("click", function () {
  nightModeOff();
});

nightModeOnBtn.addEventListener("click", function () {
  nightModeOn();
});

function nightModeOn() {
  document.querySelector("body").classList.add("nightmode-on");
  document.querySelector("#nightmode").innerHTML = "on";
}

function nightModeOff() {
  document.querySelector("body").classList.remove("nightmode-on");
  document.querySelector("#nightmode").innerHTML = "off";
}

document.querySelector(".game-screen").classList.toggle("hide");
document.querySelector(".menu-screen-settings").classList.toggle("hide");

function MainMenu() {
  document.querySelector(".menu-screen-settings").classList.toggle("hide");
  document.querySelector(".main-menu-screen").classList.toggle("hide");
}

function settingsMenu() {
  document.querySelector(".main-menu-screen").classList.toggle("hide");
  document.querySelector(".menu-screen-settings").classList.toggle("hide");
}

function startGame() {
  document.querySelector(".game-screen").classList.toggle("hide");
  document.querySelector(".main-menu-screen").classList.toggle("hide");
  var tamagotchiName = prompt("Please, enter a name of your tamagotchi:", "");
  document.querySelector("#name").innerHTML = tamagotchiName;
  if (tamagotchiName == null || tamagotchiName.replace(/\s/g, "") == "") {
    tamagotchiName = "Tamagotchi";
    document.querySelector("#name").innerHTML = tamagotchiName;
  }
  core();
  let coreUpdate = setInterval(core, 100 * day);
  function core() {
    sleepHpCount = ((tmgch.sleep / maxSleep) * 100).toFixed(2);
    hungerHpCount = ((tmgch.hunger / maxHunger) * 100).toFixed(2);
    playHpCount = ((tmgch.play / maxPlay) * 100).toFixed(2);
    energyHpCount = ((tmgch.play / maxPlay) * 100).toFixed(2);
    skillLevelHpCount = ((tmgch.play / maxPlay) * 100).toFixed(2);

    score++;
    scoreBar.innerHTML = score;

    if (tmgch.sleep >= maxSleep + (maxSleep / 100) * 20) {
      tmgch.sleep = maxSleep + (maxSleep / 100) * 20;
    }

    if (tmgch.hunger >= maxHunger + (maxHunger / 100) * 20) {
      tmgch.hunger = maxHunger + (maxHunger / 100) * 20;
    }

    if (tmgch.play >= maxPlay + (maxPlay / 100) * 20) {
      tmgch.play = maxPlay + (maxPlay / 100) * 20;
    }
    if (tmgch.energy >= maxEnergy + (maxEnergy / 100) * 20) {
      tmgch.energy = maxEnergy + (maxEnergy / 100) * 20;
    }
    if (tmgch.skillLevel >= maxSkillLevel + (maxSkillLevel / 100) * 20) {
      tmgch.skillLevel = maxSkillLevel + (maxSkillLevel / 100) * 20;
    }
    if (tmgch.energy >= maxEnergy + (maxEnergy / 100) * 20) {
      tmgch.energy = maxEnergy + (maxEnergy / 100) * 20;
    }

    if ((tmgch.sleep / maxSleep) * 100 > 100) {
      sleepHpCount = 100;
    }
    if ((tmgch.hunger / maxHunger) * 100 > 100) {
      hungerHpCount = 100;
    }
    if ((tmgch.play / maxPlay) * 100 > 100) {
      playHpCount = 100;
    }
    if ((tmgch.energy / maxEnergy) * 100 > 100) {
      energyHpCount = 100;
    }
    if ((tmgch.skillLevel / maxSkillLevel) * 100 > 100) {
      skillLevelHpCount = 100;
    }

    sleepHp.innerHTML = sleepHpCount;
    hungerHp.innerHTML = hungerHpCount;
    playHp.innerHTML = playHpCount;
    energyHp.innerHTML = energyHpCount;
    skillLevelHp.innerHTML = skillLevelHpCount;

    tmgch.tick();
    var petimage = ["1.gif", "happy.gif", "sad.gif", "dead.gif"];
    const container = document.getElementById("t-body");
    if (sleepHpCount <= 50) {
      document.getElementById("petimage").src = "sad.gif";
    } else if (sleepHpCount >= 80) {
      document.getElementById("petimage").src = "happy.gif";
    } else {
      document.getElementById("petimage").src = "1.gif";
    }

    if (hungerHpCount <= 50) {
      document.getElementById("petimage").src = "sad.gif";
    } else if (hungerHpCount >= 80) {
      document.getElementById("petimage").src = "happy.gif";
    } else {
      document.getElementById("petimage").src = "1.gif";
    }

    if (playHpCount <= 50) {
      document.getElementById("petimage").src = "sad.gif";
    } else if (playHpCount >= 80) {
      document.getElementById("petimage").src = "happy.gif";
    } else {
      document.getElementById("petimage").src = "1.gif";
    }
    if (playHpCount <= 0 || sleepHpCount <= 0 || hungerHpCount <= 0) {
      playHpCount = 0;
      sleepHpCount = 0;
      hungerHpCount = 0;
      skillLevelHpCount = 0;
      energyHpCount = 0;
      clearInterval(coreUpdate);
      document.getElementById("petimage").src = "dead.gif";
      alert("Your score: " + score);
    }
    const petImageElement = document.getElementById("petimage");
    petImageElement.style.width = "30%";
  }
}
