set(LIVREEQ_HEADERS
  Channel.h
  Client.h
  Config.h
  ConnectionDefinitions.h
  Error.h
  events/EqEventHandlerFactory.h
  events/EqEventHandlerFactory.h
  events/EqEventInfo.h
  events/Events.h
  events/handlers/ChannelPointerHandler.h
  events/handlers/KeyboardHandler.h
  FrameData.h
  FrameGrabber.h
  Node.h
  Pipe.h
  render/EqContext.h
  render/RayCastRenderer.h
  settings/CameraSettings.h
  settings/FrameSettings.h
  settings/RenderSettings.h
  settings/VolumeSettings.h
  Window.h
  coTypes.h
  types.h
  )

set(LIVREEQ_SOURCES
  Channel.cpp
  Client.cpp
  Config.cpp
  Error.cpp
  events/EqEventHandler.cpp
  events/EqEventHandlerFactory.cpp
  events/handlers/ChannelPointerHandler.cpp
  events/handlers/KeyboardHandler.cpp
  FrameData.cpp
  FrameGrabber.cpp
  Node.cpp
  Pipe.cpp
  render/EqContext.cpp
  render/RayCastRenderer.cpp
  settings/CameraSettings.cpp
  settings/FrameSettings.cpp
  settings/RenderSettings.cpp
  settings/VolumeSettings.cpp
  Window.cpp
  )
