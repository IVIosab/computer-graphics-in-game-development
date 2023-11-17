#include "dx12_renderer.h"

#include "utils/com_error_handler.h"
#include "utils/window.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <filesystem>


void cg::renderer::dx12_renderer::init()
{
	model = std::make_shared<cg::world::model>();
	model->load_obj(settings->model_path);

	camera = std::make_shared<cg::world::camera>();
	camera->set_height(static_cast<float>(settings->height));
	camera->set_width(static_cast<float>(settings->width));
	camera->set_position(float3{
			settings->camera_position[0],
			settings->camera_position[1],
			settings->camera_position[2]
	});
	camera->set_theta(settings->camera_theta);
	camera->set_phi(settings->camera_phi);
	camera->set_angle_of_view(settings->camera_angle_of_view);
	camera->set_z_near(settings->camera_z_near);
	camera->set_z_far(settings->camera_z_far);

		view_port = CD3DX12_VIEWPORT(0.f, 0.f,
									 static_cast<float>(settings->width),
									 static_cast<float>(settings->height));

		scissor_rect = CD3DX12_RECT(0, 0,
									 static_cast<LONG>(settings->width),
									 static_cast<LONG>(settings->height));

		load_pipeline();
		load_assets();
}

void cg::renderer::dx12_renderer::destroy()
{
	wait_for_gpu();
	CloseHandle(fence_event);
}

void cg::renderer::dx12_renderer::update()
{
	// TODO Lab: 3.08 Implement `update` method of `dx12_renderer`
}

void cg::renderer::dx12_renderer::render()
{
	// TODO Lab: 3.06 Implement `render` method
}

ComPtr<IDXGIFactory4> cg::renderer::dx12_renderer::get_dxgi_factory()
{
	UINT dxgi_factory_flags = 0;
#ifdef _DEBUG
	ComPtr<ID3D12Debug> debug_controller;
	if(SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debug_controller))))
	{
		debug_controller->EnableDebugLayer();
		dxgi_factory_flags |= DXGI_CREATE_FACTORY_DEBUG;
	};
#endif
	ComPtr<IDXGIFactory4> dxgi_factory;
	THROW_IF_FAILED(CreateDXGIFactory2(dxgi_factory_flags, IID_PPV_ARGS(&dxgi_factory)));
	return dxgi_factory;
}

void cg::renderer::dx12_renderer::initialize_device(ComPtr<IDXGIFactory4>& dxgi_factory)
{
	ComPtr<IDXGIAdapter1> hardware_adapter;
	dxgi_factory->EnumAdapters1(0, &hardware_adapter);
#ifdef _DEBUG
	DXGI_ADAPTER_DESC adapter_desc{};
	hardware_adapter->GetDesc(&adapter_desc);
	OutputDebugString(adapter_desc.Description);
	OutputDebugString(L"\n");
#endif
	THROW_IF_FAILED(D3D12CreateDevice(hardware_adapter.Get(),
					  D3D_FEATURE_LEVEL_11_0,
					  IID_PPV_ARGS(&device)));
}

void cg::renderer::dx12_renderer::create_direct_command_queue()
{
	D3D12_COMMAND_QUEUE_DESC queue_desc{};
	queue_desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queue_desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	THROW_IF_FAILED(device->CreateCommandQueue(&queue_desc,
							   IID_PPV_ARGS(&command_queue)));
}

void cg::renderer::dx12_renderer::create_swap_chain(ComPtr<IDXGIFactory4>& dxgi_factory)
{
	DXGI_SWAP_CHAIN_DESC1 swap_chain_desc{};
	swap_chain_desc.BufferCount = frame_number;
	swap_chain_desc.Height = settings->height;
	swap_chain_desc.Width = settings->width;
	swap_chain_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
	swap_chain_desc.SampleDesc.Count = 1;

	ComPtr<IDXGISwapChain1> temp_swap_chain;
	THROW_IF_FAILED(dxgi_factory->CreateSwapChainForHwnd(
			command_queue.Get(),
			cg::utils::window::get_hwnd(),
			&swap_chain_desc,
			nullptr,
			nullptr,
			&temp_swap_chain
			));
	dxgi_factory->MakeWindowAssociation(
			cg::utils::window::get_hwnd(),
			DXGI_MWA_NO_ALT_ENTER
			);
	temp_swap_chain.As(&swap_chain);
	frame_index = swap_chain->GetCurrentBackBufferIndex();
}

void cg::renderer::dx12_renderer::create_render_target_views()
{
	// TODO Lab: 3.04 Create a descriptor heap for render targets
	// TODO Lab: 3.04 Create render target views
}

void cg::renderer::dx12_renderer::create_depth_buffer()
{
}

void cg::renderer::dx12_renderer::create_command_allocators()
{
	// TODO Lab: 3.06 Create command allocators and a command list
}

void cg::renderer::dx12_renderer::create_command_list()
{
	// TODO Lab: 3.06 Create command allocators and a command list
}


void cg::renderer::dx12_renderer::load_pipeline()
{
	ComPtr<IDXGIFactory4> dxgi_factory = get_dxgi_factory();
	initialize_device(dxgi_factory);
	create_direct_command_queue();
	create_swap_chain(dxgi_factory);
	// TODO Lab: 3.04 Create render target views
}

D3D12_STATIC_SAMPLER_DESC cg::renderer::dx12_renderer::get_sampler_descriptor()
{
	D3D12_STATIC_SAMPLER_DESC sampler_desc{};
	return sampler_desc;
}

void cg::renderer::dx12_renderer::create_root_signature(const D3D12_STATIC_SAMPLER_DESC* sampler_descriptors, UINT num_sampler_descriptors)
{
	// TODO Lab: 3.05 Create a descriptor table and a root signature
}

std::filesystem::path cg::renderer::dx12_renderer::get_shader_path(const std::string& shader_name)
{
	// TODO Lab: 3.05 Compile shaders
	return "";
}

ComPtr<ID3DBlob> cg::renderer::dx12_renderer::compile_shader(const std::filesystem::path& shader_path, const std::string& entrypoint, const std::string& target)
{
	// TODO Lab: 3.05 Compile shaders
	return nullptr;
}

void cg::renderer::dx12_renderer::create_pso(const std::string& shader_name)
{
	// TODO Lab: 3.05 Compile shaders
	// TODO Lab: 3.05 Setup a PSO descriptor and create a PSO
}

void cg::renderer::dx12_renderer::create_resource_on_upload_heap(ComPtr<ID3D12Resource>& resource, UINT size, const std::wstring& name)
{
	// TODO Lab: 3.03 Implement resource creation on upload heap
}

void cg::renderer::dx12_renderer::create_resource_on_default_heap(ComPtr<ID3D12Resource>& resource, UINT size, const std::wstring& name, D3D12_RESOURCE_DESC* resource_descriptor)
{
}

void cg::renderer::dx12_renderer::copy_data(const void* buffer_data, UINT buffer_size, ComPtr<ID3D12Resource>& destination_resource)
{
	// TODO Lab: 3.03 Implement map, unmap, and copying data to the resource
}

void cg::renderer::dx12_renderer::copy_data(const void* buffer_data, const UINT buffer_size, ComPtr<ID3D12Resource>& destination_resource, ComPtr<ID3D12Resource>& intermediate_resource, D3D12_RESOURCE_STATES state_after, int row_pitch, int slice_pitch)
{
}

D3D12_VERTEX_BUFFER_VIEW cg::renderer::dx12_renderer::create_vertex_buffer_view(const ComPtr<ID3D12Resource>& vertex_buffer, const UINT vertex_buffer_size)
{
	// TODO Lab: 3.04 Create vertex buffer views
	return D3D12_VERTEX_BUFFER_VIEW{};
}

D3D12_INDEX_BUFFER_VIEW cg::renderer::dx12_renderer::create_index_buffer_view(const ComPtr<ID3D12Resource>& index_buffer, const UINT index_buffer_size)
{
	// TODO Lab: 3.04 Create index buffer views
	return D3D12_INDEX_BUFFER_VIEW{};
}

void cg::renderer::dx12_renderer::create_shader_resource_view(const ComPtr<ID3D12Resource>& texture, D3D12_CPU_DESCRIPTOR_HANDLE cpu_handler)
{
}

void cg::renderer::dx12_renderer::create_constant_buffer_view(const ComPtr<ID3D12Resource>& buffer, D3D12_CPU_DESCRIPTOR_HANDLE cpu_handler)
{
	// TODO Lab: 3.04 Create a constant buffer view
}

void cg::renderer::dx12_renderer::load_assets()
{
	// TODO Lab: 3.05 Create a descriptor table and a root signature
	// TODO Lab: 3.05 Setup a PSO descriptor and create a PSO
	// TODO Lab: 3.06 Create command allocators and a command list

	// TODO Lab: 3.04 Create a descriptor heap for a constant buffer

	// TODO Lab: 3.03 Allocate memory for vertex and index buffers
	// TODO Lab: 3.03 Create committed resources for vertex, index and constant buffers on upload heap
	// TODO Lab: 3.03 Copy resource data to suitable resources
	// TODO Lab: 3.04 Create vertex buffer views
	// TODO Lab: 3.04 Create index buffer views

	// TODO Lab: 3.04 Create a constant buffer view

	// TODO Lab: 3.07 Create a fence and fence event
}


void cg::renderer::dx12_renderer::populate_command_list()
{
	// TODO Lab: 3.06 Implement `populate_command_list` method
}


void cg::renderer::dx12_renderer::move_to_next_frame()
{
	// TODO Lab: 3.07 Implement `move_to_next_frame` method
}

void cg::renderer::dx12_renderer::wait_for_gpu()
{
	// TODO Lab: 3.07 Implement `wait_for_gpu` method
}


void cg::renderer::descriptor_heap::create_heap(ComPtr<ID3D12Device>& device, D3D12_DESCRIPTOR_HEAP_TYPE type, UINT number, D3D12_DESCRIPTOR_HEAP_FLAGS flags)
{
	// TODO Lab: 3.04 Implement `create_heap`, `get_cpu_descriptor_handle`, `get_gpu_descriptor_handle`, and `get` methods of `cg::renderer::descriptor_heap`
}

D3D12_CPU_DESCRIPTOR_HANDLE cg::renderer::descriptor_heap::get_cpu_descriptor_handle(UINT index) const
{
	// TODO Lab: 3.04 Implement `create_heap`, `get_cpu_descriptor_handle`, `get_gpu_descriptor_handle`, and `get` methods of `cg::renderer::descriptor_heap`
	return D3D12_CPU_DESCRIPTOR_HANDLE{};
}

D3D12_GPU_DESCRIPTOR_HANDLE cg::renderer::descriptor_heap::get_gpu_descriptor_handle(UINT index) const
{
	// TODO Lab: 3.04 Implement `create_heap`, `get_cpu_descriptor_handle`, `get_gpu_descriptor_handle`, and `get` methods of `cg::renderer::descriptor_heap`
	return D3D12_GPU_DESCRIPTOR_HANDLE{};
}
ID3D12DescriptorHeap* cg::renderer::descriptor_heap::get() const
{
	// TODO Lab: 3.04 Implement `create_heap`, `get_cpu_descriptor_handle`, `get_gpu_descriptor_handle`, and `get` methods of `cg::renderer::descriptor_heap`
	return nullptr;
}
