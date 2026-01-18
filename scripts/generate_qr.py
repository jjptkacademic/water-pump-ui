import qrcode
from PIL import Image
import requests
from io import BytesIO
import sys

def generate_qr_with_logo(url, logo_url, output_path="qr_code_with_logo.png"):
    try:
        # 1. Generate QR Code
        # We use High Error Correction (ERROR_CORRECT_H) to ensure the QR code 
        # is still readable even with the logo covering part of it.
        qr = qrcode.QRCode(
            version=1,
            error_correction=qrcode.constants.ERROR_CORRECT_H,
            box_size=10,
            border=4,
        )
        qr.add_data(url)
        qr.make(fit=True)

        qr_img = qr.make_image(fill_color="black", back_color="white").convert('RGBA')

        # 2. Load Logo
        print(f"Downloading logo from {logo_url}...")
        response = requests.get(logo_url)
        response.raise_for_status()
        logo = Image.open(BytesIO(response.content))

        # Ensure logo is RGBA
        logo = logo.convert('RGBA')

        # 3. Resize logo
        # The logo should not cover more than ~30% of the QR code area
        # A safe bet is around 20-25% of the width
        qr_width, qr_height = qr_img.size
        logo_size = int(qr_width * 0.25)
        
        logo = logo.resize((logo_size, logo_size), Image.Resampling.LANCZOS)
        
        # 4. Calculate position to center the logo
        pos = ((qr_width - logo_size) // 2, (qr_height - logo_size) // 2)
        
        # 5. Paste logo
        # Create a white background for the logo so it doesn't blend with the QR code black blocks awkwardly
        # (Optional, but looks cleaner usually)
        bg_size = int(logo_size * 1.1)
        logo_bg = Image.new('RGBA', (bg_size, bg_size), 'white')
        bg_pos = ((qr_width - bg_size) // 2, (qr_height - bg_size) // 2)
        
        # Paste white background first
        qr_img.paste(logo_bg, bg_pos, logo_bg)
        # Paste logo on top
        qr_img.paste(logo, pos, logo)

        # 6. Save
        qr_img.save(output_path)
        print(f"Success! QR code saved to: {output_path}")

    except Exception as e:
        print(f"An error occurred: {e}")

if __name__ == "__main__":
    # Configuration
    target_url = input("Enter the URL to generate QR for (default: https://github.com): ") or "https://github.com"
    
    # GitHub Logo URL (Publicly available)
    # Using the PNG version of the "Invertocat" or standard Mark
    github_logo_url = "https://github.githubassets.com/images/modules/logos_page/GitHub-Mark.png"
    
    output_filename = "github_qr.png"
    
    print(f"\nGenerating QR code for: {target_url}")
    generate_qr_with_logo(target_url, github_logo_url, output_filename)
