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
	rtv_heap.create_heap(device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, frame_number);
	for(UINT i =0; i< frame_number; i++){
		THROW_IF_FAILED(swap_chain->GetBuffer(
				i,
				IID_PPV_ARGS(&render_targets[i])));
		device->CreateRenderTargetView(
				render_targets[i].Get(),
				nullptr,
				rtv_heap.get_cpu_descriptor_handle(i)
				);
		std::wstring name(L"Render target ");
		name += std::to_wstring(i);
		render_targets[i]->SetName(name.c_str());
	}
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
	create_render_target_views();
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
	THROW_IF_FAILED(device->CreateComittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3D12_RESOURCE_DESC::Buffer(size),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&resource)
			));
	if(!name.empty()){
		resource->SetName(name.c_str());
	}
}

void cg::renderer::dx12_renderer::create_resource_on_default_heap(ComPtr<ID3D12Resource>& resource, UINT size, const std::wstring& name, D3D12_RESOURCE_DESC* resource_descriptor)
{
}

void cg::renderer::dx12_renderer::copy_data(const void* buffer_data, UINT buffer_size, ComPtr<ID3D12Resource>& destination_resource)
{
	UINT8 buffer_data_begin;
	CD3DX12_RANGE read_range(0, 0);
	THROW_IF_FAILED(
			destination_resource->Map(0, &read_range,
									  reinterpret_cast<void **>(&buffer_data_begin))
			);
	memcpy(buffer_data_begin, buffer_data, buffer_size);
	destination_resource->Unmap(0, 0);
}

void cg::renderer::dx12_renderer::copy_data(const void* buffer_data, const UINT buffer_size, ComPtr<ID3D12Resource>& destination_resource, ComPtr<ID3D12Resource>& intermediate_resource, D3D12_RESOURCE_STATES state_after, int row_pitch, int slice_pitch)
{
}

D3D12_VERTEX_BUFFER_VIEW cg::renderer::dx12_renderer::create_vertex_buffer_view(const ComPtr<ID3D12Resource>& vertex_buffer, const UINT vertex_buffer_size)
{
	D3D12_VERTEX_BUFFER_VIEW view{};
	view.BufferLocation = vertex_buffer->GetGPUVirtualAddress();
	view.StrideInBytes = sizeof(vertex);
	view.SizeInBytes = vertex_buffer_size;
	return view;
}

D3D12_INDEX_BUFFER_VIEW cg::renderer::dx12_renderer::create_index_buffer_view(const ComPtr<ID3D12Resource>& index_buffer, const UINT index_buffer_size)
{
	D3D12_INDEX_BUFFER_VIEW view{};
	view.BufferLocation = index_buffer->GetGPUVirtualAddress();
	view.SizeInBytes = index_buffer_size;
	view.Format = DXGI_FORMAT_R32_UINT;
	return view;
}

void cg::renderer::dx12_renderer::create_shader_resource_view(const ComPtr<ID3D12Resource>& texture, D3D12_CPU_DESCRIPTOR_HANDLE cpu_handler)
{
}

void cg::renderer::dx12_renderer::create_constant_buffer_view(const ComPtr<ID3D12Resource>& buffer, D3D12_CPU_DESCRIPTOR_HANDLE cpu_handler)
{
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbv_desc{};
	cbv_desc.BufferLocation = buffer->GetGPUVirtualAddress();
	cbv_desc.SizeInBytes = (sizeof(cb) + 255) & ~255;

	device->CreateConstantBufferView(&cbv_desc, cpu_handler);
}

void cg::renderer::dx12_renderer::load_assets()
{
	// TODO Lab: 3.05 Create a descriptor table and a root signature
	// TODO Lab: 3.05 Setup a PSO descriptor and create a PSO
	// TODO Lab: 3.06 Create command allocators and a command list

	cbv_srv_heap.create_heap(
			device,
			D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
			1,
			D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE
			);

	size_t num_shapes = model->get_vertex_buffers().size();
	vertex_buffers.resize(num_shapes);
	vertex_buffer_views.resize(num_shapes);
	index_buffers.resize(num_shapes);
	index_buffers_views.resize(num_shapes);
	for (size_t i = 0; i< model->get_vertex_buffers().size(); i++){
		//Vertex buffer
		auto vertex_buffer_data = model->get_vertex_buffers()[i];
		const UINT vertex_buffer_size = static_cast<UINT>(
				vertex_buffer_data->get_size_in_bytes()
		);

		std::wstring vertex_buffer_name(L"Vertex buffer ");
		vertex_buffer_name += std::to_wstring(i);

		create_resource_on_upload_heap(vertex_buffers[i],
									   vertex_buffer_size,
									   vertex_buffer_name);

		copy_data(vertex_buffer_data->get_data(),
				  vertex_buffer_size,
				  vertex_buffers[i]);
		vertex_buffer_views[i] =
				create_vertex_buffer_view(vertex_buffers[i],
										  vertex_buffer_size);

		//Index buffer
		auto index_buffer_data = model->get_index_buffers()[i];
		const UINT index_buffer_size = static_cast<UINT>(
				index_buffer_data->get_size_in_bytes()
		);

		std::wstring index_buffer_name(L"Index buffer ");
		index_buffer_name += std::to_wstring(i);

		create_resource_on_upload_heap(index_buffers[i],
									   index_buffer_size,
									   index_buffer_name);


		copy_data(index_buffer_data->get_data(),
				  index_buffer_size,
				  index_buffers[i]);

		index_buffer_views[i] = create_index_buffer_view(
				index_buffers[i],
				index_buffer_size);
	}

	// Constant buffer
	std::wstring const_buffer_name(L"Constant buffer");
	create_resource_on_upload_heap(constant_buffer,
								   64*1024,
								   const_buffer_name);
	copy_data(&cb, sizeof(cb), constant_buffer);
	CD3DX12_RANGE read_range(0, 0);
	THROW_IF_FAILED(constant_buffer->Map(0, &read_range,
										 reinterpret_cast<void**>(
												 &constant_buffer_data_begin
												 )));

	create_constant_buffer_view(constant_buffer,
								cbv_srv_heap.get_cpu_descriptor_handle(0));

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
	D3D12_DESCRIPTOR_HEAP_DESC heap_desc{};
	heap_desc.NumDescriptors = number;
	heap_desc.Type = type;
	heap_desc.Flags = flags;

	THROW_IF_FAILED(device->CreateDescriptorHeap(
			&heap_desc,
			IID_PPV_ARGS(&heap)));
	descriptor_size = device->GetDescriptorHandleIncrementSize(type);
}

D3D12_CPU_DESCRIPTOR_HANDLE cg::renderer::descriptor_heap::get_cpu_descriptor_handle(UINT index) const
{
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(
			heap->GetCPUDescriptorHandleForHeapStart(),
			static_cast<INT>(index),
			descriptor_size);
}

D3D12_GPU_DESCRIPTOR_HANDLE cg::renderer::descriptor_heap::get_gpu_descriptor_handle(UINT index) const
{
	return CD3DX12_GPU_DESCRIPTOR_HANDLE(
			heap->GetGPUDescriptorHandleForHeapStart(),
			static_cast<INT>(index),
			descriptor_size);
}
ID3D12DescriptorHeap* cg::renderer::descriptor_heap::get() const
{
	return heap.Get();
}
