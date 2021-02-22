/*
 * Copyright (C) 2014 Patrick Mours. All rights reserved.
 * License: https://github.com/crosire/reshade#license
 */

#pragma once

#include "runtime.hpp"
#include "render_d3d12.hpp"

namespace reshade::d3d12
{
	class runtime_d3d12 : public api::api_object_impl<IDXGISwapChain3 *, runtime>
	{
		static const uint32_t NUM_IMGUI_BUFFERS = 4;

	public:
		runtime_d3d12(device_impl *device, command_queue_impl *queue, IDXGISwapChain3 *swapchain);
		~runtime_d3d12();

		api::device *get_device() override { return _device_impl; }
		api::command_queue *get_command_queue() override { return _queue_impl; }

		bool on_init();
		bool on_init(const DXGI_SWAP_CHAIN_DESC &desc);
		void on_reset();
		void on_present();
		void on_present(ID3D12Resource *backbuffer, HWND hwnd);

		bool capture_screenshot(uint8_t *buffer) const override;

		void update_texture_bindings(const char *semantic, api::resource_view_handle srv) override;

	private:
		bool init_effect(size_t index) override;
		void unload_effect(size_t index) override;
		void unload_effects() override;

		bool init_texture(texture &texture) override;
		void upload_texture(const texture &texture, const uint8_t *pixels) override;
		void destroy_texture(texture &texture) override;
		void generate_mipmaps(const struct tex_data *impl);

		void render_technique(technique &technique) override;

		device_impl *const _device_impl;
		const com_ptr<ID3D12Device> _device;
		command_queue_impl *const _queue_impl;
		const com_ptr<ID3D12CommandQueue> _queue;
		command_list_immediate_impl *const _cmd_impl;

		com_ptr<ID3D12PipelineState> _mipmap_pipeline;
		com_ptr<ID3D12RootSignature> _mipmap_signature;

		UINT _swap_index = 0;
		DXGI_FORMAT _backbuffer_format = DXGI_FORMAT_UNKNOWN;
		std::vector<com_ptr<ID3D12Resource>> _backbuffers;
		com_ptr<ID3D12Resource> _backbuffer_texture;
		com_ptr<ID3D12DescriptorHeap> _backbuffer_rtvs;
		com_ptr<ID3D12DescriptorHeap> _depthstencil_dsvs;

		HMODULE _d3d_compiler = nullptr;
		com_ptr<ID3D12Resource> _effect_stencil;
		std::vector<struct effect_data> _effect_data;

		std::unordered_map<std::string, D3D12_CPU_DESCRIPTOR_HANDLE> _texture_semantic_bindings;

#if RESHADE_GUI
		bool init_imgui_resources();
		void render_imgui_draw_data(ImDrawData *data) override;

		struct imgui_resources
		{
			com_ptr<ID3D12PipelineState> pipeline;
			com_ptr<ID3D12RootSignature> signature;

			com_ptr<ID3D12Resource> indices[NUM_IMGUI_BUFFERS];
			com_ptr<ID3D12Resource> vertices[NUM_IMGUI_BUFFERS];
			int num_indices[NUM_IMGUI_BUFFERS] = {};
			int num_vertices[NUM_IMGUI_BUFFERS] = {};
		} _imgui;
#endif
	};
}
