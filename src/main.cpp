#include <SFML/Graphics.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Glsl.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Shader.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstdio>
#include <imgui.h>
#include <imgui-SFML.h>

//CONSTANTS
const int WINDOW_HEIGHT = 1000;
const int WINDOW_WIDTH = 1000;
const int FRACTAL_HEIGHT = WINDOW_HEIGHT;
const int FRACTAL_WIDTH = WINDOW_WIDTH;
//SHADER PARAMETERS
static double zoom_factor = 0.3;
static int fractal_type = 1; // 0 - Mandelbrot; 1 - Julia Set ; 2 - Burning ship
static ImVec2 mandel_constant(0.0f,0.0f);
static ImVec2 julia_constant(0.258f,0.001f);                
static float precision = 750;
static float threshold = 50;
static float offset[2] = {0.0f, 0.0f};

//PARAMETERS
static double zoom_speed = 0.002;

//STATE
static bool dragging = false;
static float dragStart[2] = {0.0f, 0.0f};
static sf::Vector2f mouse_position = sf::Vector2f{};
static bool infinite_zoom = false;
static bool show_pointer = true;


int main() {

  //Setup of window
  sf::RenderWindow window(sf::VideoMode(FRACTAL_WIDTH,FRACTAL_HEIGHT), "Fractals");
  if (!ImGui::SFML::Init(window)) {
    return 1;
  };

  
  //Setup of time tracking. 
  sf::Clock deltaClock;

  //Simple shape the shader will draw on.
  sf::RectangleShape shaderOutput(sf::Vector2f(window.getSize()));
  sf::CircleShape circle(5);
  circle.setFillColor(sf::Color::Red);

  sf::Shader shader;
  if (!shader.loadFromFile("data/shader.frag", sf::Shader::Fragment)) {
    return -1;
  }

  while (window.isOpen())
  {
      //Event handling
      sf::Event event;
      while (window.pollEvent(event)) {
        ImGui::SFML::ProcessEvent(window, event);
        ImGuiIO& im = ImGui::GetIO();
        
          if (event.type == sf::Event::Closed) {
            window.close();
          }
          if (event.type == sf::Event::MouseMoved) {
            mouse_position = window.mapPixelToCoords({event.mouseMove.x, event.mouseMove.y});
          }

          //If i am interacting with ImGUI i dont want to care about mouse events.
          if (im.WantCaptureMouse) {
            break;
          }

          //Zoom + Drag handling
          if (event.type == sf::Event::MouseButtonPressed) {
            //Drag start
            dragging = true;
            auto pos = window.mapCoordsToPixel(mouse_position);
            dragStart[0] = pos.x;
            dragStart[1] = pos.y;
          } 
          if (event.type == sf::Event::MouseButtonReleased) {
            dragging = false;
          }
          if (event.type == sf::Event::MouseWheelScrolled) {
            //Zoom
            zoom_factor += event.mouseWheelScroll.delta * 0.2 * zoom_factor;
            zoom_factor = std::max(zoom_factor, 0.1);
          }
          if (event.type == sf::Event::MouseMoved && dragging) {
            //Drag offset computation
            float posX = event.mouseMove.x;
            float posY = event.mouseMove.y;
            offset[0] += -1 * (posX - dragStart[0]) * (1/zoom_factor);
            offset[1] -= -1 * (posY - dragStart[1]) * (1/zoom_factor);
            auto pos = window.mapCoordsToPixel(mouse_position);
            dragStart[0] = pos.x;
            dragStart[1] = pos.y;
          }
      }
      if (infinite_zoom) {
        zoom_factor += zoom_speed * zoom_factor;
      }

      //Giving params to my shader.
      shader.setUniform("u_resolution", sf::Glsl::Vec2{ window.getSize() });
      shader.setUniform("zoom_factor", (float) zoom_factor);
      shader.setUniform("fractal_type", fractal_type);
      shader.setUniform("precision", precision);
      shader.setUniform("threshold", threshold);
      shader.setUniform("offset", sf::Glsl::Vec2{offset[0], offset[1]});


      ImVec2* constant = &mandel_constant;
      if (fractal_type == 1) {
        constant = &julia_constant;
      }
      shader.setUniform("u_constant", sf::Glsl::Vec2{(*constant).x, (*constant).y});

      ImGui::SFML::Update(window, deltaClock.restart());
      const char* fractal_types[3] = {"Mandelbrot", "Julia", "Burning ship"};
      ImGui::Begin("Fractals config");
      ImGui::Combo("Fractal type", &fractal_type, fractal_types, IM_ARRAYSIZE(fractal_types));
      ImGui::SliderFloat("Precision", &precision, 25.0, 10000.0);
      ImGui::SliderFloat("Threshold", &threshold, 25.0, 1000.0);
      if (fractal_type == 0 || fractal_type == 0) {
        ImGui::DragFloat2("Constant", (float*) &mandel_constant, 0.005f, -2.0, 2.0);
      }
      if (fractal_type == 1) {
        ImGui::DragFloat2("Constant", (float*) &julia_constant, 0.005f, -2.0, 2.0);
      }
      ImGui::Checkbox("Show zoom pointer", &show_pointer);
      ImGui::InputDouble("Zoom", &zoom_factor);

      ImGui::InputDouble("Infinite zoom speed", &zoom_speed);
      ImGui::Checkbox("Infinite zoom", &infinite_zoom);
      if (ImGui::Button("Reset zoom")) {
        zoom_factor = 0.3;
      }
      ImGui::End();
    
    

      //rendering part
      window.clear(sf::Color::Black);
      window.draw(shaderOutput, &shader);
      if (show_pointer) {
      
      circle.setPosition(window.getSize().x/2.0 - circle.getRadius(), window.getSize().y/2.0 - circle.getRadius());
      window.draw(circle);
      }
      ImGui::SFML::Render(window);
      window.display();
      
    
  }
  ImGui::SFML::Shutdown();
}
  
