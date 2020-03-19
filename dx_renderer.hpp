#pragma once
#include <d3d11.h>
#include <d2d1.h>
#include <d2d1_2.h>
#include <dwrite.h>
#include <dcomp.h>
#include <wrl.h>

#include "other_globals.hpp"

#pragma comment( lib, "dxgi" )
#pragma comment( lib, "d2d1" )
#pragma comment( lib, "d3d11" )
#pragma comment( lib, "dcomp" )
#pragma comment( lib, "dwrite" )

template <typename T>
using uq_object = Microsoft::WRL::ComPtr<T>;

#define RET_CHK(x) if ( x != S_OK ) return
#define RET_CHK2(x) if ( x != S_OK ) { impl::memory->set_thread_info( game_window_handle, remote_window_thread_info ); return; }

class dx_renderer
{
public:
	explicit dx_renderer( const HWND game_window_handle )
	{
		if ( !game_window_handle )
			return;

		/* create a window for our cheat, to perform thread swap on */
		_window_name.reserve( 16u );
		std::generate_n( std::back_inserter( _window_name ), 16u, [ ]
						 {
							 thread_local std::mt19937_64 mersenne_engine( std::random_device{}( ) );
							 const std::uniform_int_distribution<> distribution( 97, 122 ); // 'a', 'z'
							 return static_cast< uint8_t >( distribution( mersenne_engine ) );
						 } );

		WNDCLASS window_class
		{
			0,
			[ ]( const HWND window, const UINT message, const WPARAM wparam, const LPARAM lparam ) -> LRESULT { return DefWindowProcA( window, message, wparam, lparam ); },
			0,
			0,
			GetModuleHandleA( nullptr ),
			nullptr,
			nullptr,
			nullptr,
			nullptr,
			_window_name.c_str( )
		};

		RegisterClassA( &window_class );

		_window_handle = CreateWindowExA( 0, _window_name.c_str( ), "", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, nullptr, nullptr, window_class.hInstance, nullptr );

		if ( !_window_handle )
			return;

		RECT rect{};
		GetClientRect( game_window_handle, &rect );

		impl::width = static_cast< float >( rect.right - rect.left );
		impl::height = static_cast< float >( rect.bottom - rect.top );

		/* now initialize d3d */
		RET_CHK( D3D11CreateDevice( nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, D3D11_CREATE_DEVICE_BGRA_SUPPORT, nullptr, 0, D3D11_SDK_VERSION, &d3d11_device, nullptr, nullptr ) );
		RET_CHK( d3d11_device->QueryInterface( IID_PPV_ARGS( &dxgi_device ) ) );
		
		DXGI_SWAP_CHAIN_DESC1 swapchain_description
		{
			static_cast< UINT >( impl::width ),
			static_cast< UINT >( impl::height ),
			DXGI_FORMAT_B8G8R8A8_UNORM,
			FALSE,
			DXGI_SAMPLE_DESC{1, 0},
			DXGI_USAGE_RENDER_TARGET_OUTPUT,
			2,
			DXGI_SCALING_STRETCH,
			DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL,
			DXGI_ALPHA_MODE_PREMULTIPLIED,
			0
		};

		RET_CHK( CreateDXGIFactory2( 0, IID_PPV_ARGS( &dxgi_factory ) ) );
		RET_CHK( dxgi_factory->CreateSwapChainForComposition( d3d11_device.Get( ), &swapchain_description, nullptr, &dxgi_swapchain ) );
		RET_CHK( D2D1CreateFactory<ID2D1Factory2>( D2D1_FACTORY_TYPE_MULTI_THREADED, &d2d_factory ) );
		RET_CHK( d2d_factory->CreateDevice( dxgi_device.Get( ), &d2d_device ) );
		RET_CHK( d2d_device->CreateDeviceContext( D2D1_DEVICE_CONTEXT_OPTIONS_ENABLE_MULTITHREADED_OPTIMIZATIONS, &d2d_context ) );
		RET_CHK( dxgi_swapchain->GetBuffer( 0, IID_PPV_ARGS( &dxgi_surface ) ) );
		
		const D2D1_BITMAP_PROPERTIES1 bitmap_properties
		{
			D2D1_PIXEL_FORMAT{DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED},
			0.f,
			0.f,
			D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
			nullptr
		};

		RET_CHK( d2d_context->CreateBitmapFromDxgiSurface( dxgi_surface.Get( ), bitmap_properties, &d2d_bitmap ) );

		d2d_context->SetTarget( d2d_bitmap.Get( ) );
		d2d_context->SetAntialiasMode( D2D1_ANTIALIAS_MODE_ALIASED );

		RET_CHK( d2d_context->CreateSolidColorBrush( D2D1::ColorF( D2D1::ColorF::Black ), &d2d_brush ) );
		RET_CHK( DWriteCreateFactory( DWRITE_FACTORY_TYPE_SHARED, __uuidof( IDWriteFactory ), reinterpret_cast< IUnknown** >( dwrite_factory.GetAddressOf( ) ) ) );
		RET_CHK( dwrite_factory->CreateTextFormat( L"Tahoma", nullptr, DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 12.f, L"en-us", &dwrite_font ) );
		RET_CHK( dwrite_factory->CreateTextFormat( L"Tahoma", nullptr, DWRITE_FONT_WEIGHT_BOLD, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 12.f, L"en-us", &dwrite_font_bold ) );

		// concept:
		// swapping the KTHREAD in the tagWnd structure of each window momentarily, composing a window, returning.

		const auto local_window_thread_info = impl::memory->get_thread_info( _window_handle );
		const auto remote_window_thread_info = impl::memory->get_thread_info( game_window_handle );

		impl::memory->set_thread_info( game_window_handle, local_window_thread_info );

		RET_CHK2( DCompositionCreateDevice( dxgi_device.Get( ), IID_PPV_ARGS( &dcomp_device ) ) )
		RET_CHK2( dcomp_device->CreateTargetForHwnd( game_window_handle, TRUE, &dcomp_target ) )
		RET_CHK2( dcomp_device->CreateVisual( &dcomp_visual ) )
		RET_CHK2( dcomp_visual->SetContent( dxgi_swapchain.Get( ) ) )
		RET_CHK2( dcomp_target->SetRoot( dcomp_visual.Get( ) ) )
		RET_CHK2( dcomp_device->Commit( ) )
		RET_CHK2( dcomp_device->WaitForCommitCompletion( ) )

		impl::memory->set_thread_info( game_window_handle, remote_window_thread_info );
	}

	void begin_scene( )
	{
		d2d_context->BeginDraw( );
		d2d_context->Clear( );
	}


	void rectangle(float x, float y, float w, float h, const glm::uvec3& color)
	{
		d2d_brush.Get()->SetColor(D2D1::ColorF(D3DCOLOR_RGBA(color.x, color.y, color.z, 255), 1.f));
		d2d_context.Get()->DrawRectangle(D2D1::RectF(x, y, x + w, y + h), d2d_brush.Get(), 1.2f, nullptr);
	}

	void Line(const glm::vec2& start, const glm::vec2& end, glm::uvec3 color, float thickness)
	{
		d2d_brush.Get()->SetColor(D2D1::ColorF(D3DCOLOR_RGBA(color.x, color.y, color.z, 255), 1.f));
		d2d_context.Get()->DrawLine({ start.x, start.y }, { end.x, end.y }, d2d_brush.Get(), thickness);
	}

	template <typename ...Args>
	void draw_text( const glm::vec2& pos, const glm::uvec3& clr, const float sz, const std::wstring_view text, Args&&... args )
	{
		const auto size = static_cast< std::size_t >( std::swprintf( nullptr, 0, text.data( ), std::forward<Args>( args )... ) + 1 );

		const std::unique_ptr<wchar_t[ ]> buffer( new wchar_t[ size ] );
		std::swprintf( buffer.get( ), size, text.data( ), std::forward<Args>( args )... );

		const auto str = std::wstring( buffer.get( ), buffer.get( ) + size - 1 );
		const auto str_len = static_cast< std::uint32_t >( str.size( ) );

		IDWriteTextLayout* dwrite_layout = nullptr;
		RET_CHK( dwrite_factory->CreateTextLayout( str.c_str( ), str_len, dwrite_font.Get( ), impl::width, impl::height, &dwrite_layout ) );

		const DWRITE_TEXT_RANGE range
		{
			0,
			str_len
		};

		dwrite_layout->SetFontSize( sz, range );

		static const auto black_color = D2D1::ColorF( D2D1::ColorF::Black );

		// this is a retarded way to render an outline, but that's what I gotta do if I don't want to write 500 lines to use sprites & glyphs.
		d2d_brush->SetColor( black_color );

		const auto x = pos.x;
		const auto y = pos.y;

		d2d_context->DrawTextLayout( D2D1::Point2F( x - 1, y ), dwrite_layout, d2d_brush.Get( ) );
		d2d_context->DrawTextLayout( D2D1::Point2F( x + 1, y ), dwrite_layout, d2d_brush.Get( ) );
		d2d_context->DrawTextLayout( D2D1::Point2F( x, y - 1 ), dwrite_layout, d2d_brush.Get( ) );
		d2d_context->DrawTextLayout( D2D1::Point2F( x, y + 1 ), dwrite_layout, d2d_brush.Get( ) );

		d2d_brush->SetColor( D2D1::ColorF( D3DCOLOR_RGBA( clr.x, clr.y, clr.z, 255 ) ) );
		d2d_context->DrawTextLayout( D2D1::Point2F( x, y ), dwrite_layout, d2d_brush.Get( ) );

		dwrite_layout->Release( );
	}

	template <bool bold>
	void draw_text( const glm::vec2& pos, const glm::uvec3& clr, const float sz, const std::wstring_view text )
	{
		const auto str_len = static_cast< std::uint32_t >( text.size( ) );

		IDWriteTextLayout* dwrite_layout = nullptr;
		RET_CHK( dwrite_factory->CreateTextLayout( text.data( ), str_len, bold ? dwrite_font_bold.Get( ) : dwrite_font.Get( ), impl::width, impl::height, &dwrite_layout ) );

		const DWRITE_TEXT_RANGE range
		{
			0,
			str_len
		};

		dwrite_layout->SetFontSize( sz, range );

		static const auto black_color = D2D1::ColorF( D2D1::ColorF::Black );

		// this is a retarded way to render an outline, but that's what I gotta do if I don't want to write 500 lines to use sprites & glyphs.
		d2d_brush->SetColor( black_color );

		const auto x = pos.x;
		const auto y = pos.y;

		d2d_context->DrawTextLayout( D2D1::Point2F( x - 1, y ), dwrite_layout, d2d_brush.Get( ) );
		d2d_context->DrawTextLayout( D2D1::Point2F( x + 1, y ), dwrite_layout, d2d_brush.Get( ) );
		d2d_context->DrawTextLayout( D2D1::Point2F( x, y - 1 ), dwrite_layout, d2d_brush.Get( ) );
		d2d_context->DrawTextLayout( D2D1::Point2F( x, y + 1 ), dwrite_layout, d2d_brush.Get( ) );

		d2d_brush->SetColor( D2D1::ColorF( D3DCOLOR_RGBA( clr.x, clr.y, clr.z, 255 ) ) );
		d2d_context->DrawTextLayout( D2D1::Point2F( x, y ), dwrite_layout, d2d_brush.Get( ) );

		dwrite_layout->Release( );
	}

	void end_scene( )
	{
		d2d_context->EndDraw( );
		dxgi_swapchain->Present( 0, 0 );
	}

	~dx_renderer( )
	{
		DestroyWindow( _window_handle );
		UnregisterClassA( _window_name.c_str( ), GetModuleHandleA( nullptr ) );
	}
private:
	std::string _window_name = {};
	HWND _window_handle = nullptr;
private:
	uq_object<ID3D11Device> d3d11_device = nullptr;

	uq_object<IDXGIDevice> dxgi_device = nullptr;
	uq_object<IDXGISwapChain1> dxgi_swapchain = nullptr;
	uq_object<IDXGIFactory2> dxgi_factory = nullptr;
	uq_object<IDXGISurface2> dxgi_surface = nullptr;

	uq_object<ID2D1Device1> d2d_device = nullptr;
	uq_object<ID2D1DeviceContext1> d2d_context = nullptr;
	uq_object<ID2D1Factory2> d2d_factory = nullptr;
	uq_object<ID2D1Bitmap1> d2d_bitmap = nullptr;
	uq_object<ID2D1SolidColorBrush> d2d_brush = nullptr;

	uq_object<IDWriteFactory> dwrite_factory = nullptr;
	uq_object<IDWriteTextFormat> dwrite_font = nullptr;
	uq_object<IDWriteTextFormat> dwrite_font_bold = nullptr;

	uq_object<IDCompositionDevice> dcomp_device = nullptr;
	uq_object<IDCompositionVisual> dcomp_visual = nullptr;
	uq_object<IDCompositionTarget> dcomp_target = nullptr;
};