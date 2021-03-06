#include "Globals.h"
#include "Application.h"
#include "ModulePlayer.h"
#include "Primitive.h"
#include "PhysVehicle3D.h"
#include "PhysBody3D.h"
#include <gl/GL.h>
#include <gl/GLU.h>

ModulePlayer::ModulePlayer(Application* app, bool start_enabled) : Module(app, start_enabled), vehicle(NULL)
{
	turn = acceleration = brake = 0.0f;
}

ModulePlayer::~ModulePlayer()
{}

// Load assets
bool ModulePlayer::Start()
{
	LOG("Loading player");


	// Fx's
	//engine_fx = App->audio->LoadFx("Sound/engine.wav");
	//App->audio->Change_Fx_Volume(engine_fx, 40);
	//App->audio->PlayFx(engine_fx, -1);
	

	// lights
	/*glEnable(GL_LIGHTING);
	focus.Init();
	focus.Active(true);
	focus.ambient = { 0.4f, 0.5f, 0.6f, 1.0f };
	focus.diffuse = { 0.6f, 0.5f, 0.4f, 1.0f };
	focus.ref */

	VehicleInfo car;


	// Car properties ----------------------------------------
	car.chassis_size.Set(1.875, 0.75, 3.375);
	car.chassis_offset.Set(0, 0.75, 0);

	/*car.cabin_size.Set(2.0, 0.3, 2); 
	car.cabin_offset.Set(0, 1.8, -1);

	car.front_light_radius_height = (0.2, 0.4); 
	car.front_light_offset = (1.5, 0, 3);*/


	car.rear_wing_size.Set(3.6, 0.3, 0.225);
	car.rear_wing_offset.Set(0, 1.95, -1.875);

	car.rear_wing_support_size.Set(0.15, 0.75, 0.15); 
	car.rear_wing_support_offset.Set(0, 1.425, -1.8375); 

	car.rear_wing_flag_size.Set(0.075, 0.825, 0.825); 
	car.rear_wing_flag_offset.Set(1.875, 1.725, -1.875); 

	car.mass = 250.0f;
	car.suspensionStiffness = 10.f;//18.0f;  // 15.88f; // default to 5.88 / 10.0 offroad / 50.0 sports car / 200.0 F1 car
	car.suspensionCompression = 0.4f;//0.83f; // default to 0.83
	car.suspensionDamping = 0.5f;  // 0.88f; // default to 0.88 / 0..1 0 bounces / 1 rigid / recommended 0.1...0.3
	car.maxSuspensionTravelCm = 150.0f;		 // default to 500 cm suspension can be compressed
	car.frictionSlip = 5.f; //4.5f; // 50.5; // defaults to 10.5 / friction with the ground. 0.8 should be good but high values feels better (kart 1000.0)
	car.maxSuspensionForce = 6000.0f; // defaults to 6000 / max force to the chassis

	// Wheel properties ---------------------------------------
	float front_connection_height = 0.5625f;
	float front_wheel_radius = 0.5625f;
	float front_wheel_width = 0.5325f;
	float front_suspensionRestLength = 0.5f;

	float rear_connection_height = 0.6f;
	float rear_wheel_radius = 0.6f;
	float rear_wheel_width = 0.5625f;
	float rear_suspensionRestLength = 0.6f; 

	
	// car properties

	float half_rear_width = car.chassis_size.x*1.0f;
	float half_front_width = car.chassis_size.x*0.9f;

	float half_length = car.chassis_size.z*0.8f;
	
	vec3 direction(0,-1,0);
	vec3 axis(-1,0,0);
	
	car.num_wheels = 4;
	car.wheels = new Wheel[4];

	// FRONT-LEFT ------------------------
	car.wheels[0].connection.Set(half_front_width - 0.3f * front_wheel_width, front_connection_height, half_length - front_wheel_radius);
	car.wheels[0].direction = direction;
	car.wheels[0].axis = axis;
	car.wheels[0].suspensionRestLength = front_suspensionRestLength;
	car.wheels[0].radius = front_wheel_radius;
	car.wheels[0].width = front_wheel_width;
	car.wheels[0].front = true;
	car.wheels[0].drive = true;
	car.wheels[0].brake = false;
	car.wheels[0].steering = true;

	// FRONT-RIGHT ------------------------
	car.wheels[1].connection.Set(-half_front_width + 0.3f * front_wheel_width, front_connection_height, half_length - front_wheel_radius);
	car.wheels[1].direction = direction;
	car.wheels[1].axis = axis;
	car.wheels[1].suspensionRestLength = front_suspensionRestLength;
	car.wheels[1].radius = front_wheel_radius;
	car.wheels[1].width = front_wheel_width;
	car.wheels[1].front = true;
	car.wheels[1].drive = true;
	car.wheels[1].brake = false;
	car.wheels[1].steering = true;

	// REAR-LEFT ------------------------
	car.wheels[2].connection.Set(half_rear_width - 0.3f * rear_wheel_width, rear_connection_height, -half_length + rear_wheel_radius);
	car.wheels[2].direction = direction;
	car.wheels[2].axis = axis;
	car.wheels[2].suspensionRestLength = rear_suspensionRestLength;
	car.wheels[2].radius = rear_wheel_radius;
	car.wheels[2].width = rear_wheel_width;
	car.wheels[2].front = false;
	car.wheels[2].drive = false;
	car.wheels[2].brake = true;
	car.wheels[2].steering = false;

	// REAR-RIGHT ------------------------
	car.wheels[3].connection.Set(-half_rear_width + 0.3f * rear_wheel_width, rear_connection_height, -half_length + rear_wheel_radius);
	car.wheels[3].direction = direction;
	car.wheels[3].axis = axis;
	car.wheels[3].suspensionRestLength = rear_suspensionRestLength;
	car.wheels[3].radius = rear_wheel_radius;
	car.wheels[3].width = rear_wheel_width;
	car.wheels[3].front = false;
	car.wheels[3].drive = false;
	car.wheels[3].brake = true;
	car.wheels[3].steering = false;

	car.rear_transm_size.Set(3, 0.15, 0.15);
	car.rear_transm_offset.Set(car.wheels[3].connection.x + 1.65f, car.wheels[2].connection.y - rear_wheel_radius / 2, car.wheels[2].connection.z);

	vehicle = App->physics->AddVehicle(car);
	vehicle->SetPos(0, 0, -180);
	vehicle->Set_Orientation(_PI / 2, {0, 1, 0});
	 
	
	return true;
}

// Unload assets
bool ModulePlayer::CleanUp()
{
	LOG("Unloading player");

	return true;
}

// Update: draw background
update_status ModulePlayer::Update(float dt)
{

	turn = acceleration = brake = 0.0f;

	// handle light

	focus.position.x = vehicle->GetPos().x;
	focus.position.y = vehicle->GetPos().y + 2;
	focus.position.z = vehicle->GetPos().z;
	
	focus.Render();

	// handle special keys
	Keys(); 
	// handle player controls
	if (App->scene_intro->game_state == GameState::GO)
		MoveControls();

	
	/*if (vehicle->GetKmh() > 0) {
		acceleration += inertia; 
	}
	else if (vehicle->GetKmh() < 0) {
		acceleration += inertia / 4;
	}*/


	vehicle->ApplyEngineForce(acceleration);
	vehicle->Turn(turn);
	vehicle->Brake(brake);

	vehicle->Render();

	if(lock_camera)
		CameraLogic(dt);


	return UPDATE_CONTINUE;
}

void ModulePlayer::MoveControls()
{
	inertia = (-vehicle->info.mass * vehicle->GetKmh()) / 50 + vehicle->GetKmh();

	if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
	{
		//App->audio->Change_Fx_Volume(engine_fx, MIX_MAX_VOLUME);    // louder sound 


		if (vehicle->GetKmh() <= MAX_SPEED)
			acceleration = MAX_ACCELERATION;

		if (App->input->GetKey(SDL_SCANCODE_P) == KEY_REPEAT) {
			vehicle->info.rear_wing_size.y = 0.05f;                   // open rear wing (DRS)

			if (vehicle->GetKmh() <= MAX_SPEED_DRS)
				acceleration = MAX_ACC_DRS;
		}
	}

	else {

		//App->audio->Change_Fx_Volume(engine_fx, 40);  // default engine sound

		vehicle->info.rear_wing_size.y = 0.4f;  // close DRS

		if (vehicle->GetKmh() > 0) {
			acceleration += inertia;
		}

	}

	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
	{
		if (turn < TURN_DEGREES)
			turn += TURN_DEGREES;
		//	acceleration /= 4;   // nerf
	}

	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
	{
		if (turn > -TURN_DEGREES)
			turn -= TURN_DEGREES;
		//	acceleration /= 4;
	}

	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
	{

		if (vehicle->GetKmh() > 0) {                      // brake 
			brake = BRAKE_POWER;
		}
		else {
			acceleration = -MAX_ACCELERATION / 4;             // go backwards 
		}

	}
	else if (vehicle->GetKmh() < 0) {
		acceleration += inertia / 4;
	}
}


void ModulePlayer::Keys() {

	// locks / unlocks camera to vehicle
	if (App->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN) {
		lock_camera = !lock_camera;
	}
	
	// returns to last reached checkpoint, if any
	if (App->input->GetKey(SDL_SCANCODE_R) == KEY_DOWN)
	{
		if (App->scene_intro->check_points.Count() > 0)
		{
			for (int i = App->scene_intro->check_points.Count() - 1; i >= 0; --i)
			{
				if (App->scene_intro->check_points[i].active)
				{
					mat4x4 trm;
					App->scene_intro->check_points[i].body->GetTransform(&trm);
					vehicle->SetTransform(&trm);
					vehicle->Set_Speed({ 0,0,0 });
					// for every time the player repositions the car, penalize with plus time
					App->scene_intro->AddPenalizationTime(10000);
					// play sfx
					App->audio->PlayFx(App->scene_intro->cameraMoveSFX);
					break;
				}
			}
		}
	}

	// NEW game
	if (App->input->GetKey(SDL_SCANCODE_N) == KEY_DOWN)
		App->scene_intro->game_state = GameState::PREPARATION;
	
	// switch game states
	if (App->input->GetKey(SDL_SCANCODE_F9) == KEY_DOWN)
	{
		App->audio->PlayMusic("", 0.0f); // stops music
		App->audio->PlayFx(App->scene_intro->winSFX);
		App->scene_intro->game_state = GameState::WIN;
	}

	//// debug positions

	//if (App->input->GetKey(SDL_SCANCODE_L) == KEY_DOWN) {
	//	vehicle->SetPos(-180, 0, -180);
	//}

	//if (App->input->GetKey(SDL_SCANCODE_G) == KEY_DOWN) {
	//	vehicle->SetPos(-10, 0, 177);
	//}

}

void ModulePlayer::CameraLogic(float dt)
{
	vec3 currCarPos = vehicle->GetPos();
	vec3 forwardDir = vehicle->GetForwardVector();
	vec3 desiredCamDistance = currCarPos - (forwardDir * 7.f);

	float wantedHeight = currCarPos.y + 4.f;

	float damp = 5.0f;
	btVector3 currentCam;
	currentCam.setValue(App->camera->Position.x, App->camera->Position.y, App->camera->Position.z);
	btVector3 desiredCamDist;
	desiredCamDist.setValue(desiredCamDistance.x, desiredCamDistance.y + wantedHeight, desiredCamDistance.z);

	btVector3 newCamera = lerp(currentCam, desiredCamDist, damp * dt);
	
	App->camera->Position = { newCamera.getX(), newCamera.getY(), newCamera.getZ() };
	currCarPos.y += 3.f; // tweaks a little upper from car position to look at with some inclination
	App->camera->LookAt(currCarPos);
	
}





