// spaceshooter.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <SFML/Graphics.hpp>
using namespace sf;
#include<list>;

const int W = 1200;
const int H = 800;
float degtorad = 0.017453f;


class animation {

public:
	float frame, speed;
	Sprite sprite;
	std::vector<IntRect>Frames;

	animation() {}
	animation(Texture &t, int x, int y, int w, int h, int count, float Speed)
	{

		frame = 0;
		speed = Speed;


		for (int i = 0; i < count; i++) {
			Frames.push_back(IntRect(x + i * w, y, w, h));

			sprite.setTexture(t);
			sprite.setOrigin(w / 2, h / 2);
			sprite.setTextureRect(Frames[0]);
		}
	}

	void update() {


		frame += speed;
		int n = Frames.size();

		if (frame >= n)frame -= n;


		if (n > 0)sprite.setTextureRect(Frames[int(frame)]);


	}
	bool isEnd()
	{
		return frame + speed >= Frames.size();
	}


};


class Entity {

public:
	float x, y, dx, dy, r, angle;
	bool life;
	std::string name;
	animation ani;
	Entity() { life = 1; }

	void set(animation &a, int X, int Y, float Angle, int radius = 1) {
			ani = a;
			x = X;
			y = Y;
		
			angle = Angle;
			r = radius;
	}

	virtual void update() ;

	void draw(RenderWindow &window) {

		ani.sprite.setPosition(x, y);
		ani.sprite.setRotation(angle + 90);
		window.draw(ani.sprite);

		
	}
	virtual ~Entity() {};
};

class asteroid :public Entity {

public:
	asteroid() {

		dx = rand() % 8 - 4;
		dy= rand() % 8 - 4;
		name = "asteroid";
	}
	void update() {

		x += dx;
		y += dy;
		if (x > W)x = 0;
		if (x < 0)x = W;
		if (y > H)y = 0;
		if (y < 0)y = H;


	}

};

class bullet :public Entity {

public:
	
	bullet() {
		name = "bullet";
	}

	void update() {
		//angle += rand() % 7 - 3;
		dx = cos(angle*degtorad) * 6;
		dy = sin(angle*degtorad) * 6;
		
		x += dx;
		y += dy;
		if (x > W || x<0 || y>H || y < 0) life = 0;

	}
};


class player :public Entity
{
public:
	bool thrust;

	player() {
		name = "player";
	}

	void update() {

		if (thrust) {

			dx += cos(angle*degtorad)*0.2;
			dy += sin(angle*degtorad)*0.2;

		}
		else {
			dx *= 0.99;
			dy *= 0.99;
		}


		int maxspeed = 5;
		float speed = sqrt(dx*dx + dy * dy);
		if (speed > maxspeed) {

			dx *= maxspeed / speed;
			dy *= maxspeed / speed;
		}
		x += dx;
		y += dy;
		if (x > W)x = 0; if (x < 0)x = W;
		if (y > H) y = 0; if (y < 0) y = H;
	}

};
bool collision(Entity *a, Entity *b) {

	return((b->x - a->x)*(b->x - a->x) + (b->y - a->y)*(b->y - a->y)
		<
		(a->r + b->r)*(a->r + b->r)
		);
}



int main()
{
	srand(time(0));
	RenderWindow window(VideoMode(W, H),"Space Shooter");
	window.setFramerateLimit(60);


	Texture t1, t2,t3,t4,t5;
	t2.loadFromFile("images/background.jpg");
	t1.loadFromFile("images/spaceship.png");
	t3.loadFromFile("images/explosions/type_A.png");
	t4.loadFromFile("images/rock.png");
	t5.loadFromFile("images/fire_blue.png");

	t1.setSmooth(true);
	t2.setSmooth(true);


	animation sbullet(t5, 0, 0, 32, 64, 16, 0.8);

	Sprite playeR(t1), bg(t2),explosion(t3);
	//player.setTextureRect(IntRect(40, 0, 40, 40)); /// (a,b,c,d)  a,b sfml window posiitoning .....c,d cordiantes for part of image file
	//player.setOrigin(20, 20);



	animation rock(t4, 0, 0, 64, 64, 16, 0.2);  //16 rocks in rock.png each of rock 64*64
	//rock.sprite.setPosition(400, 400);

	animation splayer(t1, 40, 0, 40, 40, 1, 0);
	splayer.sprite.setPosition(600, 400);
	std::list<Entity*>entities;

	for (int i = 0; i <15 ; i++) {

		asteroid *a = new asteroid();
		a->set(rock, rand() % W, rand() % H, rand() % 360, 25);
		entities.push_back(a);

	}

	float x = 300, y = 300;
	float dx = 0, dy = 0, angle = 0;
	bool thrust;


	player *p = new player();
	p->set(splayer, 200, 200, 0, 20);
	entities.push_back(p);

	while (window.isOpen()) {
		Event e;
		while (window.pollEvent(e)) {

			if (e.type == Event::Closed)
				window.close();

			if (e.type == Event::KeyPressed) {
				if (e.key.code == Keyboard::Space) {
					bullet *b=new bullet();
					b->set(sbullet,p-> x, p->y, p->angle, 10);
					entities.push_back(b);
				}

			}

		}
		if (Keyboard::isKeyPressed(Keyboard::Right ))p->angle += 3;
		if (Keyboard::isKeyPressed(Keyboard::Left)) p -> angle -= 3;
		if (Keyboard::isKeyPressed(Keyboard::Up))p -> thrust = true;
		else thrust = false;
		

		for(auto a:entities)
			for(auto b:entities)
				if(a->name=="asteroid" && b->name=="bullet")
					if (collision(a, b)) {

						a->life = false;
						b->life = false;
					}



		

		for (auto i = entities.begin(); i != entities.end();) {

			Entity *e = *i;
			e->update();
			e->ani.update();

			if (e ->life == false) {
				i = entities.erase(i);
				delete e;
			}
			else i++;
			
		}


		rock.update();




		window.clear();
		window.draw(bg);
		//window.draw(player);
		for (auto i : entities)i->draw(window);
		//window.draw(rock.sprite);
		window.display();


	}


	return 0;


}

