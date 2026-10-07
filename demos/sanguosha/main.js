const canvas = document.getElementById("gameCanvas");
const ctx = canvas.getContext("2d");
const logBox = document.getElementById("log");

let currentTurn = "player";
let selectedCardIndex = -1;

// 玩家和敌人（后面可扩展为数组）
const player = {
  name: "你",
  hp: 3,
  maxHp: 3,
  hand: ["杀", "桃", "决斗", "乐不思蜀"],
  delayed: []
};

const ai = {
  name: "敌人",
  hp: 3,
  maxHp: 3,
  hand: ["杀", "杀", "桃"],
  delayed: []
};

// 卡牌定义
const cardEffects = {
  "杀": {
    type: "attack",
    effect: (self, target) => {
      target.hp -= 1;
      log(`${self.name} 使用了【杀】，${target.name} 受到1点伤害！`);
    }
  },
  "桃": {
    type: "heal",
    effect: (self, _) => {
      if (self.hp < self.maxHp) {
        self.hp += 1;
        log(`${self.name} 使用了【桃】，恢复1点体力！`);
      } else {
        log(`${self.name} 体力已满，无法使用【桃】！`);
      }
    }
  },
  "决斗": {
    type: "duel",
    effect: (self, target) => {
      log(`${self.name} 发起【决斗】！`);
      let turn = 0;
      while (true) {
        const attacker = turn % 2 === 0 ? self : target;
        const defender = turn % 2 === 0 ? target : self;
        const idx = attacker.hand.indexOf("杀");
        if (idx === -1) {
          defender.hp -= 1;
          log(`${attacker.name} 无法出【杀】，${defender.name} 受到1点伤害（决斗）！`);
          break;
        } else {
          attacker.hand.splice(idx, 1);
          log(`${attacker.name} 出了【杀】应对决斗`);
        }
        turn++;
      }
    }
  },
  "乐不思蜀": {
    type: "delayed",
    effect: (self, target) => {
      target.delayed.push("乐不思蜀");
      log(`${self.name} 对 ${target.name} 使用了【乐不思蜀】`);
    }
  }
};

// 渲染函数
function drawGame() {
  ctx.clearRect(0, 0, canvas.width, canvas.height);
  ctx.font = "20px sans-serif";
  ctx.fillStyle = "black";
  ctx.fillText(`${player.name} HP: ${player.hp}`, 20, 30);
  ctx.fillText(`${ai.name} HP: ${ai.hp}`, 600, 30);

  // 判定区
  if (player.delayed.length > 0)
    ctx.fillText(`你的判定区：${player.delayed.join(", ")}`, 20, 60);
  if (ai.delayed.length > 0)
    ctx.fillText(`敌人判定区：${ai.delayed.join(", ")}`, 600, 60);

  // 手牌
  ctx.fillText("你的手牌：", 20, 100);
  player.hand.forEach((card, i) => {
    ctx.fillStyle = "#fff";
    ctx.fillRect(20 + i * 110, 120, 100, 140);
    ctx.strokeRect(20 + i * 110, 120, 100, 140);
    ctx.fillStyle = "#000";
    ctx.font = "24px serif";
    ctx.fillText(card, 40 + i * 110, 190);
  });
}

function log(text) {
  logBox.innerText = text;
}

canvas.addEventListener("click", (e) => {
  if (currentTurn !== "player") return;

  const rect = canvas.getBoundingClientRect();
  const mx = e.clientX - rect.left;
  const my = e.clientY - rect.top;

  for (let i = 0; i < player.hand.length; i++) {
    const x = 20 + i * 110;
    const y = 120;
    if (
      mx >= x && mx <= x + 100 &&
      my >= y && my <= y + 140
    ) {
      const cardName = player.hand[i];
      const card = cardEffects[cardName];
      if (!card) return;
      if (card.type === "heal" || card.type === "delayed") {
        card.effect(player, ai);
      } else {
        card.effect(player, ai);
      }
      player.hand.splice(i, 1);
      checkWin();
      endTurn();
      break;
    }
  }

  drawGame();
});

// 判定区处理：乐不思蜀 → 有50%概率跳过出牌阶段
function checkDelayed(player) {
  if (player.delayed.includes("乐不思蜀")) {
    const fate = Math.random() < 0.5;
    if (!fate) {
      log(`${player.name} 判定失败，被【乐不思蜀】，跳过出牌阶段！`);
      return false;
    } else {
      log(`${player.name} 判定成功，摆脱【乐不思蜀】！`);
      player.delayed = player.delayed.filter(c => c !== "乐不思蜀");
    }
  }
  return true;
}

// AI 回合
function aiTurn() {
  setTimeout(() => {
    if (!checkDelayed(ai)) {
      currentTurn = "player";
      drawGame();
      return;
    }

    const cardName = ai.hand.find(c => c === "杀" || c === "决斗" || c === "乐不思蜀");
    if (!cardName) {
      log("敌人跳过了回合！");
    } else {
      ai.hand.splice(ai.hand.indexOf(cardName), 1);
      const card = cardEffects[cardName];
      card.effect(ai, player);
    }

    checkWin();
    currentTurn = "player";
    drawGame();
  }, 1200);
}

// 回合切换
function endTurn() {
  currentTurn = "ai";
  drawGame();
  aiTurn();
}

// 胜负判断
function checkWin() {
  if (player.hp <= 0) {
    log("你输了！");
    currentTurn = "end";
  } else if (ai.hp <= 0) {
    log("你赢了！");
    currentTurn = "end";
  }
}

// 初始渲染
drawGame();
log("你的回合，请点击出牌！");
