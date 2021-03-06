#include "PhysVehicle3D.h"
#include "Primitive.h"
#include "Bullet/include/btBulletDynamicsCommon.h"

// ----------------------------------------------------------------------------
VehicleInfo::~VehicleInfo()
{
	//if(wheels != NULL)
		//delete wheels;
}

// ----------------------------------------------------------------------------
PhysVehicle3D::PhysVehicle3D(btRigidBody* body, btRaycastVehicle* vehicle, const VehicleInfo& info) : PhysBody3D(body), vehicle(vehicle), info(info)
{
}

// ----------------------------------------------------------------------------
PhysVehicle3D::~PhysVehicle3D()
{
	delete vehicle;
}

// ----------------------------------------------------------------------------
void PhysVehicle3D::Render()
{
	Cylinder wheel;

	wheel.color = {0.2f, 0.2f, 0.2f, 1.0f};

	for(int i = 0; i < vehicle->getNumWheels(); ++i)
	{
		wheel.radius = info.wheels[i].radius;
		wheel.height = info.wheels[i].width;

		vehicle->updateWheelTransform(i);
		vehicle->getWheelInfo(i).m_worldTransform.getOpenGLMatrix(&wheel.transform);

		wheel.Render();
	}

	// chassis

	Cube chassis(info.chassis_size.x, info.chassis_size.y, info.chassis_size.z);
	vehicle->getChassisWorldTransform().getOpenGLMatrix(&chassis.transform);
	btQuaternion q = vehicle->getChassisWorldTransform().getRotation();
	btVector3 offset(info.chassis_offset.x, info.chassis_offset.y, info.chassis_offset.z);
	offset = offset.rotate(q.getAxis(), q.getAngle());

	chassis.transform.M[12] += offset.getX();
	chassis.transform.M[13] += offset.getY();
	chassis.transform.M[14] += offset.getZ();
	chassis.color = Violet;

	chassis.Render();


	// rear wing 

	Cube rear_wing(info.rear_wing_size.x, info.rear_wing_size.y, info.rear_wing_size.z);

	vehicle->getChassisWorldTransform().getOpenGLMatrix(&rear_wing.transform);
	btVector3 offset_q_rw(info.rear_wing_offset.x, info.rear_wing_offset.y, info.rear_wing_offset.z);
	offset_q_rw = offset_q_rw.rotate(q.getAxis(), q.getAngle());

	rear_wing.transform.M[12] += offset_q_rw.getX();
	rear_wing.transform.M[13] += offset_q_rw.getY();
	rear_wing.transform.M[14] += offset_q_rw.getZ();
	rear_wing.color = Black;
	rear_wing.Render(); 

	// rear wing support

	Cube rear_wing_support(info.rear_wing_support_size.x, info.rear_wing_support_size.y, info.rear_wing_support_size.z);

	vehicle->getChassisWorldTransform().getOpenGLMatrix(&rear_wing_support.transform);

	btVector3 offset_q_rw_sup(info.rear_wing_support_offset.x, info.rear_wing_support_offset.y, info.rear_wing_support_offset.z);
	offset_q_rw_sup = offset_q_rw_sup.rotate(q.getAxis(), q.getAngle());

	rear_wing_support.transform.M[12] += offset_q_rw_sup.getX();
	rear_wing_support.transform.M[13] += offset_q_rw_sup.getY();
	rear_wing_support.transform.M[14] += offset_q_rw_sup.getZ(); 
	rear_wing_support.color = Black;
	
	
	rear_wing_support.Render();



	// rear wing flag right

	Cube rear_wing_flag_right(info.rear_wing_flag_size.x, info.rear_wing_flag_size.y, info.rear_wing_flag_size.z);

	vehicle->getChassisWorldTransform().getOpenGLMatrix(&rear_wing_flag_right.transform);
	btVector3 offset_q_rw_flag_r(info.rear_wing_flag_offset.x, info.rear_wing_flag_offset.y, info.rear_wing_flag_offset.z);
	offset_q_rw_flag_r = offset_q_rw_flag_r.rotate(q.getAxis(), q.getAngle());

	rear_wing_flag_right.transform.M[12] += offset_q_rw_flag_r.getX();
	rear_wing_flag_right.transform.M[13] += offset_q_rw_flag_r.getY();
	rear_wing_flag_right.transform.M[14] += offset_q_rw_flag_r.getZ();
	rear_wing_flag_right.color = Violet;
	rear_wing_flag_right.Render();


	// rear wing flag left

	Cube rear_wing_flag_left(info.rear_wing_flag_size.x, info.rear_wing_flag_size.y, info.rear_wing_flag_size.z);

	vehicle->getChassisWorldTransform().getOpenGLMatrix(&rear_wing_flag_left.transform);
	btVector3 offset_q_rw_flag_l(-info.rear_wing_flag_offset.x, info.rear_wing_flag_offset.y, info.rear_wing_flag_offset.z);
	offset_q_rw_flag_l = offset_q_rw_flag_l.rotate(q.getAxis(), q.getAngle());

	rear_wing_flag_left.transform.M[12] += offset_q_rw_flag_l.getX();
	rear_wing_flag_left.transform.M[13] += offset_q_rw_flag_l.getY();
	rear_wing_flag_left.transform.M[14] += offset_q_rw_flag_l.getZ();
	rear_wing_flag_left.color = Violet;
	rear_wing_flag_left.Render();

	// rear transmission


	Cube rear_transm(info.rear_transm_size.x, info.rear_transm_size.y, info.rear_transm_size.z);

	vehicle->getChassisWorldTransform().getOpenGLMatrix(&rear_transm.transform);
	btVector3 offset_q_rear_transm(-info.rear_transm_offset.x, info.rear_transm_offset.y, info.rear_transm_offset.z);
	offset_q_rear_transm = offset_q_rear_transm.rotate(q.getAxis(), q.getAngle());

	rear_transm.transform.M[12] += offset_q_rear_transm.getX();
	rear_transm.transform.M[13] += offset_q_rear_transm.getY();
	rear_transm.transform.M[14] += offset_q_rear_transm.getZ();
	rear_transm.color = Black;
	rear_transm.Render();


}

// ----------------------------------------------------------------------------
void PhysVehicle3D::ApplyEngineForce(float force)
{
	for(int i = 0; i < vehicle->getNumWheels(); ++i)
	{
		if(info.wheels[i].drive == true)
		{
			vehicle->applyEngineForce(force, i);
		}
	}
}

// ----------------------------------------------------------------------------
void PhysVehicle3D::Brake(float force)
{
	for(int i = 0; i < vehicle->getNumWheels(); ++i)
	{
		if(info.wheels[i].brake == true)
		{
			vehicle->setBrake(force, i);
		}
	}
}

// ----------------------------------------------------------------------------
void PhysVehicle3D::Turn(float degrees)
{
	for(int i = 0; i < vehicle->getNumWheels(); ++i)
	{
		if(info.wheels[i].steering == true)
		{
			vehicle->setSteeringValue(degrees, i);
		}
	}
}

// ----------------------------------------------------------------------------
float PhysVehicle3D::GetKmh() const
{
	return vehicle->getCurrentSpeedKmHour();
}

vec3 PhysVehicle3D::GetForwardVector() const
{
	return { vehicle->getForwardVector().getX(), vehicle->getForwardVector().getY(),vehicle->getForwardVector().getZ() };
}