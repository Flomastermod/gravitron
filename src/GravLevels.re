open GravShared;
open SharedTypes;

open Reprocessing;

open Enemy;

let initialSpeed = isPhone ? 1. : 2.;
let sizeFactor = isPhone ? 0.8 : 1.0;

/* Enemy types */

let defaultEnemy = (~health=1, ~size=20., pos, warmup, maxTime) => {
  pos,
  color: Constants.white,
  size: size *. sizeFactor,
  warmup: (0., 50.),
  health: (health, health),
  animate: 0.,
  movement: Stationary,
  vel: (MyUtils.v0),
  dying: Normal,
  stepping: DoNothing,
  shooting: OneShot(Bullet.template(
    ~color=Constants.white,
    ~size=5. *. sizeFactor,
    ~speed=initialSpeed,
    ~damage=5,
    ()
  )),
  dodges: (0., 0.),
  missileTimer: (warmup, maxTime),
  selfDefense: None,
};

let red = (~warmup=200., pos) => {
  ...defaultEnemy(pos, warmup, 300.),
  color: Constants.red,
  size: 20. *. sizeFactor,
  shooting: OneShot(Bullet.template(
    ~color=Constants.white,
    ~size=5. *. sizeFactor,
    ~speed=initialSpeed,
    ~damage=5,
    ()
  ))
};

let blue = (~warmup=100., pos) => {
  ...defaultEnemy(pos, warmup, 300.),
  color: Reprocessing_Constants.blue,
  size: 20. *. sizeFactor,
  health: (2, 2),
  shooting: OneShot(Bullet.template(
    ~color=Utils.color(~r=150, ~g=150, ~b=255, ~a=255),
    ~size=4. *. sizeFactor,
    ~speed=initialSpeed,
    ~damage=3,
    ()
  ))
};

let smallGreen = (~warmup=0., pos) => {
  ...defaultEnemy(pos, warmup, 200.),
  color: Reprocessing_Constants.green,
  size: 15. *. sizeFactor,
  health: (3, 3),
  shooting: TripleShot(Bullet.template(
    ~color=Reprocessing.Constants.green,
    ~size=4. *. sizeFactor,
    ~speed=initialSpeed,
    ~damage=5,
    ()
  ))
};

let green = (~warmup=0., pos) => {
  ...defaultEnemy(pos, warmup, 200.),
  color: Reprocessing_Constants.green,
  size: 25. *. sizeFactor,
  health: (5, 5),
  shooting: TripleShot(Bullet.template(
    ~color=Reprocessing.Constants.green,
    ~size=7. *. sizeFactor,
    ~speed=initialSpeed,
    ~damage=10,
    ()
  ))
};

let colorPink = Reprocessing_Utils.color(~r=255, ~g=100, ~b=200, ~a=255);
let pink = (~warmup=0., pos) => {
  ...defaultEnemy(pos, warmup, 300.),
  color: colorPink,
  size: 30. *. sizeFactor,
  health: (4, 4),
  movement: GoToPosition(pos),
  vel: MyUtils.v0,
  dying: Asteroid,
  shooting: OneShot(Bullet.template(
    ~color=colorPink,
    ~size=10. *. sizeFactor,
    ~speed=initialSpeed,
    ~damage=15,
    ()
  ))
};

let color = Reprocessing.Utils.color(~r=100, ~g=150, ~b=255, ~a=255);
let scatterShooter = (~warmup=200., pos) => {
  ...defaultEnemy(pos, warmup, 300.),
  color,
  size: 35. *. sizeFactor,
  health: (10, 10),
  movement: Wander(pos),
  vel: MyUtils.v0,
  /* behavior: ScatterShot((warmup, 300.), 5, (color, 10. *. sizeFactor, initialSpeed *. 1.5, 10), (color, 7. *. sizeFactor, initialSpeed, 10)), */
  shooting: OneShot(Bullet.template(
    ~color=colorPink,
    ~size=10. *. sizeFactor,
    ~speed=initialSpeed,
    ~stepping=Bullet.Scatter(
      (0., 60.),
      5,
      Bullet.template(
        ~color,
        ~size=7. *. sizeFactor,
        ~speed=initialSpeed,
        ~damage=10,
        ()
      )
    ),
    ~damage=15,
    ()
  ))
};

/* TODO these should probably be parameterized */
let stages = (env) => {
  let w = Env.width(env) |> float_of_int;
  let h = Env.height(env) |> float_of_int;
  let q = min(w, h) /. 4.;
  let tl = (q, q);
  let tr = (w -. q, q);
  let bl = (q, h -. q);
  let br = (w -. q, h -. q);
  [|
  [|
  [red(br)],
  [red(tl), red(br)],
  [red(tl), blue(br)],
  [blue(~warmup=150., br), blue(tl)],
  [blue(~warmup=50., br), blue(~warmup=100., tl), blue(~warmup=150., tr), blue(~warmup=0., bl)],
  |],

  [|
  [smallGreen(br)],
  [red(tl), smallGreen(br), blue(tr)],
  [green(br)],
  [scatterShooter(br)],
  [pink(br)],
  |],

  [|
  [
    {
    ...red(br),
    movement: Wander(br),
    stepping: Rabbit(600., (550., 600.))
  },
    {
    ...red(bl),
    movement: Wander(bl),
    stepping: Rabbit(600., (200., 600.))
  },
    {
    ...red(tr),
    movement: Wander(tr),
    stepping: Rabbit(600., (0., 600.))
  }
  ],
  [
    {
      ...red(tl),
      /* movement: Avoider(MyUtils.v0), */
      dying: Revenge(20, Bullet.template(~color=color, ~speed=initialSpeed, ~damage=5, ~size=5. *. sizeFactor, ()))
    },
    {
      ...red(br),
      /* movement: Avoider(MyUtils.v0), */
      dying: Revenge(20, Bullet.template(~color=color, ~speed=initialSpeed, ~damage=5, ~size=5. *. sizeFactor, ()))
    },
    {
      ...red(bl),
      /* movement: Avoider(MyUtils.v0), */
      dying: Revenge(10, Bullet.template(~color=color, ~speed=initialSpeed, ~damage=5, ~size=5. *. sizeFactor, ()))
    }
  ],
  [
    {
      ...defaultEnemy(~size=20., ~health=5, tl, 0., 200.),
      shooting: OneShot(Bullet.template(
        ~color=Utils.color(~r=255, ~g=255, ~b=100, ~a=255),
        ~size=5. *. sizeFactor,
        ~speed=initialSpeed,
        ~moving=HeatSeeking(0.3, 5.),
        ~damage=5,
        ()
      ))
    },
    {
      ...defaultEnemy(~size=20., ~health=5, tr, 0., 200.),
      shooting: OneShot(Bullet.template(
        ~color=Constants.green,
        ~size=5. *. sizeFactor,
        ~speed=initialSpeed,
        ~stepping=TimeBomb((0., 60.)),
        /* ~moving=HeatSeeking(0.1), */
        ~damage=5,
        ()
      ))
    },
    {
      ...defaultEnemy(~size=20., ~health=5, br, 0., 200.),
      shooting: OneShot(Bullet.template(
        ~color=Utils.color(~r=100, ~g=200, ~b=200, ~a=255),
        ~size=10. *. sizeFactor,
        ~speed=initialSpeed,
        ~stepping=ProximityScatter(
          100.,
          5,
          Bullet.template(
            ~color=Constants.white,
            ~size=5. *. sizeFactor,
            ~speed=initialSpeed,
            ~damage=3,
            ()
          )
        ),
        ~damage=10,
        ()
      ))
    },
    {
      ...defaultEnemy(~size=20., ~health=5, bl, 0., 40.),
      shooting: Alternate(Bullet.template(
        ~color=Utils.color(~r=255, ~g=100, ~b=255, ~a=255),
        ~size=10. *. sizeFactor,
        ~speed=initialSpeed,
        ~moving=Mine(30., 100., (0., 100.)),
        /* ~moving=HeatSeeking(0.1), */
        ~damage=10,
        ()
      ), Bullet.template(
        ~color=Constants.white,
        ~size=4. *. sizeFactor,
        ~speed=initialSpeed,
        ~damage=3,
        ()
      ), false)
    },
  ], [
    {
      ...defaultEnemy(~size=20., ~health=5, bl, 0., 70.),
      movement: Avoider(300.),
      shooting: OneShot(Bullet.template(
        ~color=Utils.color(~r=255, ~g=100, ~b=255, ~a=255),
        ~size=10. *. sizeFactor,
        ~speed=initialSpeed,
        ~moving=Mine(30., 100., (0., 100.)),
        ~stepping=Shooter((0., 300.), Bullet.template(
          ~color=Constants.white,
          ~size=4. *. sizeFactor,
          ~speed=initialSpeed,
          ~damage=3,
          ()
        )),
        ~damage=10,
        ()
      ))
    },
  ]
  |]
|]
};

let makePhoneStages = (env) => {
  let w = float_of_int(Env.width(env)) *. phoneScale;
  let h = float_of_int(Env.height(env)) *. phoneScale;
  let v = w /. 4.;
  [|
    [|
    [red((w /. 2., v))],
    [red(~warmup=50., (w /. 2., v)), red((w /. 2., h -. v))],
    [red((w /. 2., v)), blue((w /. 2., h -. v))],
    [blue(~warmup=150., (w /. 2., v)), blue((w /. 2., h -. v))],
    [blue(~warmup=50., (v, v)), blue((v *. 3., v)), blue((v, h -. v)), blue((v *. 3., h -. v))],
    |],
    [|
    [smallGreen((w /. 2., v))],
    [red((v, v)), smallGreen((w -. v, h -. v)), blue((v, h -. v))],
    [green((w /. 2., h -. v))],
    [scatterShooter((w /. 2., v))],
    [pink((w /. 2., h -. v))],
    |], [|
    [
      {
        ...red(~warmup=250., (v, v)),
        movement: Wander((v, v)),
        stepping: Rabbit(600., (550., 600.))
      },
    ],
  [
    {
      ...defaultEnemy(~size=20., ~health=5, (v, h -. v), 0., 200.),
      shooting: OneShot(Bullet.template(
        ~color=Utils.color(~r=255, ~g=255, ~b=100, ~a=255),
        ~size=5. *. sizeFactor,
        ~speed=initialSpeed,
        ~moving=HeatSeeking(0.3, 5.),
        ~damage=5,
        ()
      ))
    },
      {
        ...red(~warmup=250., (v, v)),
        movement: Wander((v, v))
      },
  ],
  [
      {
        ...red(~warmup=250., (v, v)),
        movement: Wander((v, v))
      },

    {
      ...defaultEnemy(~size=20., ~health=5, (w -. v, v), 100., 200.),
      shooting: OneShot(Bullet.template(
        ~color=Constants.green,
        ~size=5. *. sizeFactor,
        ~speed=initialSpeed,
        ~stepping=TimeBomb((0., 120.)),
        /* ~moving=HeatSeeking(0.1), */
        ~damage=5,
        ()
      ))
    },

  ]
  |]
  |]
};

let getStages = (env) => isPhone ? makePhoneStages(env) : stages(env);

/* let getStages = (env) => {
  let stages = [|
    FreePlay.makeStage(env, true),
    FreePlay.makeStage(env, false)
  |];
  stages
}; */
