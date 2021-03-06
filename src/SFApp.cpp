#include "SFApp.h"

SFApp::SFApp(std::shared_ptr<SFWindow> window) : fire(0), is_running(true), sf_window(window) {
  int canvas_w, canvas_h;
  SDL_GetRendererOutputSize(sf_window->getRenderer(), &canvas_w, &canvas_h);

  app_box = make_shared<SFBoundingBox>(Vector2(canvas_w, canvas_h), canvas_w, canvas_h);
  player  = make_shared<SFAsset>(SFASSET_PLAYER, sf_window);
  auto player_pos = Point2(canvas_w/2, 22);
  player->SetPosition(player_pos);

  background  = make_shared<SFAsset>(SFASSET_BACKGROUND, sf_window);
  auto background_pos = Point2(canvas_w/2, canvas_h/2);
  background->SetPosition(background_pos);

  const int number_of_aliens = 8;
  for(int i=1; i<number_of_aliens; i++) {
    // place an alien at width/number_of_aliens * i
    auto alien = make_shared<SFAsset>(SFASSET_ALIEN, sf_window);
    auto pos   = Point2((canvas_w/number_of_aliens) * i, 375.0f);
    alien->SetPosition(pos);
    aliens.push_back(alien);
  }

  const int number_of_walls = 7;
  for(int i=1; i<number_of_walls; i++) {
    // placement of walls
    auto wall = make_shared<SFAsset>(SFASSET_WALL, sf_window);
    auto pos   = Point2((canvas_w/number_of_walls) * i, 150.0f);
    wall->SetPosition(pos);
    walls.push_back(wall);
  }

  const int number_of_walls2 = 4;
  for(int i=1; i<number_of_walls2; i++) {
    // placement of walls
    auto wall2 = make_shared<SFAsset>(SFASSET_WALL2, sf_window);
    auto pos   = Point2((canvas_w/number_of_walls2) * i, 300.0f);
    wall2->SetPosition(pos);
    walls2.push_back(wall2);
  }
  
  auto coin = make_shared<SFAsset>(SFASSET_COIN, sf_window);
  auto pos  = Point2((canvas_w/2), 450);
  coin->SetPosition(pos);
  coins.push_back(coin);
}

SFApp::~SFApp() {
}

/**
 * Handle all events that come from SDL.
 * These are timer or keyboard events.
 */
void SFApp::OnEvent(SFEvent& event) {
  SFEVENT the_event = event.GetCode();
  switch (the_event) {
  case SFEVENT_QUIT:
    is_running = false;
    break;
  case SFEVENT_UPDATE:
    OnUpdateWorld();
    OnRender();
    break;
  case SFEVENT_PLAYER_LEFT:
    player->GoWest();
    break;
  case SFEVENT_PLAYER_RIGHT:
    player->GoEast();
    break;
  case SFEVENT_PLAYER_DOWN:
    player->GoSouth();
    break;
  case SFEVENT_PLAYER_UP:
    player->GoNorth();
    break;
  case SFEVENT_FIRE:
    fire ++;
    FireProjectile();
    break;
  }
}

int SFApp::OnExecute() {
  // Execute the app
  SDL_Event event;
  while (SDL_WaitEvent(&event) && is_running) {
    // wrap an SDL_Event with our SFEvent
    SFEvent sfevent((const SDL_Event) event);
    // handle our SFEvent
    OnEvent(sfevent);
  }
}

int kills = 1;
int collectCoin = 0;

void SFApp::OnUpdateWorld() {
  // Update projectile positions
  for(auto p: projectiles) {
    if(p->IsAlive()) {p->GoNorth();
    }else{p->StopProjectiles();}
  }

  for(auto c: coins){
    if(player->CollidesWith(c)){
      c->SetNotAlive();
      player->SetNotAlive();
      std::cout << "You have gained 100 points!" << std::endl;
      std::cout << "Well done, you have completed this level and have saved one of you're crew members!" << std::endl;
      std::cout << "You have finished this game with a score of " << kills+99 << "." << std::endl;
      is_running = false;
    }
  }

  // Update enemy positions
  for(auto a : aliens) {
    // do something here
  }

  // Detect collisions
  for(auto p : projectiles) {
    for(auto a : aliens) {
      if(p->CollidesWith(a)) {
        p->HandleCollision();
        a->HandleCollision();
    std::cout << "You have killed " << kills << " Enemies!" << std::endl;
    std::cout << "You have gained 1 point!" << std::endl;
    kills = kills+1;
      }
    }
  }

  //Stops projectile when it hits a wall
  for(auto w : walls) {
    for(auto p : projectiles) {
      if(w->CollidesWith(p)) {
        w->HandleCollision();
        p->HandleCollision();
      }
    }
  }

  //Stops projectile when it hits walls2
  for(auto w2 : walls2) {
    for(auto p : projectiles) {
      if(w2->CollidesWith(p)) {
        w2->HandleCollision();
        p->HandleCollision();
      }
    }
  }
  
  //removes player after it hits a wall
  for(auto w: walls) {
    if(w->CollidesWith(player)) {
      w->HandleCollision();
      player->SetNotAlive();
      std::cout << "You are dead!" << std::endl;
      std::cout << "You have finished this game with a score of " << kills-1 << "." << std::endl;
      is_running = false;
    }
  }

   //removes player after it hits walls2
  for(auto w2: walls2) {
    if(w2->CollidesWith(player)) {
      w2->HandleCollision();
      player->SetNotAlive();
      std::cout << "You are dead!" << std::endl;
      std::cout << "You have finished this game with a score of " << kills-1 << "." << std::endl;
      is_running = false;
    }
  }
    
  // remove dead aliens
  list<shared_ptr<SFAsset>> tmp;
  for(auto a : aliens) {
    if(a->IsAlive()) {
      tmp.push_back(a);
    }
  }
  aliens.clear();
  aliens = list<shared_ptr<SFAsset>>(tmp);
  tmp.clear();

  //Collision of player and Alien
  for(auto a : aliens) {
    if( player->CollidesWith(a)) {
      a->HandleCollision();
      player->SetNotAlive();
      std::cout << "You are dead!" << std::endl;
      std::cout << "You have finished this game with a score of " << kills-1 << "." << std::endl;
      is_running = false;
    }
  }
  
  //removes collected coin
  for(auto c : coins) {
    if(c->IsAlive()){
      tmp.push_back(c);
    }
  }
  coins.clear();
  coins = list<shared_ptr<SFAsset>>(tmp);
  tmp.clear();
}

void SFApp::OnRender() {
  SDL_RenderClear(sf_window->getRenderer());

  // draw the player
  background->OnRender();
  if(player->IsAlive()) {player->OnRender();}

  for(auto p: projectiles) {
    if(p->IsAlive()) {p->OnRender();}
  }

  for(auto a: aliens) {
    if(a->IsAlive()) {a->OnRender();}
  }

  for(auto c: coins) {
    if(c->IsAlive()) {c->OnRender();}
  }

  for(auto w: walls) {
    if(w->IsAlive()) {w->OnRender();}
  }

  for(auto w2: walls2) {
    if(w2->IsAlive()) {w2->OnRender();}
  }
  
  // Switch the off-screen buffer to be on-screen
  SDL_RenderPresent(sf_window->getRenderer());
}

void SFApp::FireProjectile() {
  auto pb = make_shared<SFAsset>(SFASSET_PROJECTILE, sf_window);
  auto v  = player->GetPosition();
  pb->SetPosition(v);
  projectiles.push_back(pb);
}
